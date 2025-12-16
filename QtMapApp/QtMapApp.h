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

#ifndef QTMAPAPP_H
#define QTMAPAPP_H

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h")

class QQuickWindow;

namespace Esri::ArcGISRuntime {
class Map;
class MapQuickView;
} // namespace Esri::ArcGISRuntime

class QtMapApp : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView *mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit QtMapApp(QObject *parent = nullptr);
  ~QtMapApp() override;

  static void runQtMapOnJavaFxWindow(quintptr parentHwnd);

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView *mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView *mapView);

public:
  static QQuickWindow* s_quickWindow;
  static Esri::ArcGISRuntime::Map *s_map;
  static Esri::ArcGISRuntime::MapQuickView *s_mapView;
};

#endif // QTMAPAPP_H
