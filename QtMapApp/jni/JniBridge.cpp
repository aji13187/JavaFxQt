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

// ArcGIS Runtime types
#include "Map.h"
#include "MapQuickView.h"
#include "Point.h"
#include "SpatialReference.h"

// Qt headers
#include <QCoreApplication>
#include <QFuture>
#include <QGuiApplication>
#include <QWindow>

#include "JniBridge.h"
#include "../QtMapApp.h"

using namespace Esri::ArcGISRuntime;

static JavaVM* s_jvm = nullptr;
static jobject s_helloControllerGlobal = nullptr;
static jmethodID s_callMethod = nullptr;

namespace JniBridge {

  void initialize(JNIEnv* env, jobject helloController) {
    if (!env || !helloController) {
      return;
    }
    env->GetJavaVM(&s_jvm);
    s_helloControllerGlobal = env->NewGlobalRef(helloController);
    jclass cls = env->GetObjectClass(helloController);
    s_callMethod = env->GetMethodID(cls, "callJavaFxMethodFromQt", "(DD)V");
  }

  void shutdown(JNIEnv* env) {
    if (env && s_helloControllerGlobal) {
      env->DeleteGlobalRef(s_helloControllerGlobal);
    }
    s_helloControllerGlobal = nullptr;
    s_callMethod = nullptr;
    s_jvm = nullptr;
  }

  void callJavaFxMethodFromQt(double x, double y) {
    if (!s_jvm || !s_helloControllerGlobal || !s_callMethod) {
      return;
    }

    JNIEnv* env = nullptr;
    const jint getEnvResult = s_jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    bool attachedHere = false;
    if (getEnvResult != JNI_OK) {
      if (s_jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != JNI_OK) {
        return;
      }
      attachedHere = true;
    }

    env->CallVoidMethod(s_helloControllerGlobal, s_callMethod, x, y);

    if (attachedHere) {
      s_jvm->DetachCurrentThread();
    }
  }

} // namespace JniBridge

extern "C" {

  JNIEXPORT void JNICALL Java_com_example_javafxqt_HelloController_runQtMapOnJavaFxWindow(JNIEnv *env, jobject obj, jlong parentHwnd)
  {
    JniBridge::initialize(env, obj);
    QtMapApp::runQtMapOnJavaFxWindow(static_cast<quintptr>(parentHwnd));
  }

  JNIEXPORT void JNICALL Java_com_example_javafxqt_HelloController_setViewPoint(JNIEnv *env, jobject obj, jdouble x, jdouble y)
  {
    Q_UNUSED(env);
    Q_UNUSED(obj);
    const Point pt(x, y, QtMapApp::s_map->spatialReference());
    QtMapApp::s_mapView->setViewpointCenterAsync(pt);
  }

  JNIEXPORT void JNICALL Java_com_example_javafxqt_HelloController_setViewPointWithScale(JNIEnv *env, jobject obj, jdouble x, jdouble y, jdouble scaleValue)
  {
    Q_UNUSED(env);
    Q_UNUSED(obj);
    const Point pt(x, y, SpatialReference(QtMapApp::s_map->spatialReference()));
    QtMapApp::s_mapView->setViewpointCenterAsync(pt, scaleValue);
  }

  JNIEXPORT void JNICALL Java_com_example_javafxqt_HelloController_setViewPointRotation(JNIEnv *env, jobject obj, jdouble rotation)
  {
    Q_UNUSED(env);
    Q_UNUSED(obj);
    QtMapApp::s_mapView->setViewpointRotationAsync(rotation);
  }

  JNIEXPORT void JNICALL Java_com_example_javafxqt_HelloController_setViewPointScale(JNIEnv *env, jobject obj, jdouble scale)
  {
    Q_UNUSED(env);
    Q_UNUSED(obj);
    QtMapApp::s_mapView->setViewpointScaleAsync(scale);
  }

  JNIEXPORT void JNICALL Java_com_example_javafxqt_HelloController_closeQtApplication(JNIEnv *env, jobject obj)
  {
    Q_UNUSED(obj);
    if (!QCoreApplication::instance()) {
      return;
    }

    QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, []() {
      for (QWindow *window : QGuiApplication::topLevelWindows()) {
        window->deleteLater();
      }
    });

    QCoreApplication::quit();
    JniBridge::shutdown(env);
  }
}
