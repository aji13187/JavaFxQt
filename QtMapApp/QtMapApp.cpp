// Copyright 2025 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#include "QtMapApp.h"

// ArcGIS headers
#include "ArcGISRuntimeEnvironment.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "SpatialReference.h"

// Qt headers
#include <QDir>
#include <QFuture>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QDebug>

// JNI Bridge
#include "jni/JniBridge.h"

QQuickWindow* QtMapApp::s_quickWindow = nullptr;
Esri::ArcGISRuntime::Map* QtMapApp::s_map = nullptr;
Esri::ArcGISRuntime::MapQuickView* QtMapApp::s_mapView = nullptr;

using namespace Esri::ArcGISRuntime;

QtMapApp::QtMapApp(QObject *parent /* = nullptr */)
    : QObject(parent)
{
  s_map = new Map(BasemapStyle::ArcGISImagery, parent);
}

QtMapApp::~QtMapApp() {
}

void QtMapApp::runQtMapOnJavaFxWindow(quintptr parentHwnd)
{
  int argc = 0;
  char *argv[] = { nullptr };
  QGuiApplication app(argc, argv);

  // Use of ArcGIS location services, such as basemap styles, geocoding, and routing services,
  // requires an access token. For more information see
  // https://links.esri.com/arcgis-runtime-security-auth.

  // The following methods grant an access token:

  // 1. User authentication: Grants a temporary access token associated with a user's ArcGIS account.
  // To generate a token, a user logs in to the app with an ArcGIS account that is part of an
  // organization in ArcGIS Online or ArcGIS Enterprise.

  // 2. API key authentication: Get a long-lived access token that gives your application access to
  // ArcGIS location services. Go to the tutorial at https://links.esri.com/create-an-api-key.
  // Copy the API Key access token.

  const QString accessToken = QString("");
  if (accessToken.isEmpty())
  {
    qDebug() << "Use of ArcGIS location services, such as the basemap styles service, requires"
                          << "you to authenticate with an ArcGIS account or set the API Key property.";
  }
  else
  {
    ArcGISRuntimeEnvironment::setApiKey(accessToken);
  }

  // Production deployment of applications built with ArcGIS Maps SDK requires you to
  // license ArcGIS Maps SDK functionality. For more information see
  // https://links.esri.com/arcgis-runtime-license-and-deploy.

  // ArcGISRuntimeEnvironment::setLicense("Place license string in here");

  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.QtMapApp", 1, 0, "MapView");

  // Register the QtMapApp (QQuickItem) for QML
  qmlRegisterType<QtMapApp>("Esri.QtMapApp", 1, 0, "QtMapApp");

  // Create QQmlApplicationEngine and load the QML file
  QQmlApplicationEngine engine;
  // Add the import Path
  engine.addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));

  // Set the source
  engine.load(QUrl("qrc:/qml/main.qml"));
  if (engine.rootObjects().isEmpty())
  {
    qCritical() << "Failed to load QML file.";
    return;
  }
  QWindow *foreignWindow = QWindow::fromWinId(static_cast<WId>(parentHwnd));
  if (!foreignWindow)
  {
    qDebug() <<  "Failed to create QWindow from native window handle.";
    return;
  }

  // Set the foreign window as a parent of the QML root object
  const auto rootObjects = engine.rootObjects();
  QObject *rootObject = rootObjects.first();
  QtMapApp::s_quickWindow = qobject_cast<QQuickWindow *>(rootObject);
  if (QtMapApp::s_quickWindow)
  {
    // Set the Map as a child of the JavaFx window.
    QtMapApp::s_quickWindow->setParent(foreignWindow);
    QtMapApp::s_quickWindow->setFlags(Qt::Window | Qt::FramelessWindowHint);
    QtMapApp::s_quickWindow->setPosition(200, 190);
    foreignWindow->show();
    QtMapApp::s_quickWindow->show();
  }
  else
  {
    qDebug() << "Root object is not a QQuickWindow.";
    return;
  }

  // foreignWindow is not the child of any object, so make sure it gets deleted at some point
  // Reset static pointers and exit the Qt event loop when host (JavaFX) window is destroyed
  QObject::connect(foreignWindow, &QObject::destroyed, &app, [&app]() {
    QtMapApp::s_mapView = nullptr;
    QtMapApp::s_quickWindow = nullptr;
    QCoreApplication::quit();
  });

  app.exec();
}

MapQuickView *QtMapApp::mapView() const
{
  return QtMapApp::s_mapView;
}

// Set the view (created in QML)
void QtMapApp::setMapView(MapQuickView *mapView)
{
  if (!mapView || mapView == QtMapApp::s_mapView)
  {
    return;
  }

  QtMapApp::s_mapView = mapView;
  QtMapApp::s_mapView->setMap(QtMapApp::s_map);
  emit mapViewChanged();

  connect(QtMapApp::s_mapView, &MapQuickView::mouseClicked, this, [](QMouseEvent& mouseEvent)
  {
    const auto globalPos = mouseEvent.globalPosition();
    const auto localPos = QtMapApp::s_quickWindow->mapFromGlobal(globalPos);
    const auto point = QtMapApp::s_mapView->screenToLocation(localPos.x(), localPos.y());
    // Use JNI bridge for thread-safe callback into Java
    JniBridge::callJavaFxMethodFromQt(point.x(), point.y());
  });
}
