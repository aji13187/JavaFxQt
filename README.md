# JavaFxQt - Integrating Qt into your JavaFX apps to render the map.

Now that the ArcGIS Maps SDK for Java is [deprecated](https://www.esri.com/arcgis-blog/products/sdk-java/announcements/announcing-the-deprecation-of-arcgis-maps-sdk-for-java) and retired, you may be wondering how to migrate your apps written with the ArcGIS Maps SDK for Java. 
This workflow shows how you can keep your existing Java codebase, and swap out the map component with one implemented with the [ArcGIS Maps SDK for Qt](https://developers.arcgis.com/qt/). It also shows how to communicate between the two layers with some small examples.

This repository contains two apps that work together:
- `JavaFxApp`: a JavaFX UI that hosts a native Qt window.
- `QtMapApp`: a Qt 6 app using ArcGIS Maps SDK for Qt, built as a DLL and embedded into JavaFX via JNI.

Run the `JavaFxApp` to showcase how the Qt Map application can be embedded within a JavaFX application.
The `JavaFxApp` demonstrates basic interactions between JavaFX and the Qt Map application through JNI. This code is compatible with both Windows and macOS.
Before running, ensure that the JDK, JavaFX, Qt, and ArcGIS SDK paths are correctly configured in both the Qt and JavaFX IDEs according to your operating system.

## Prerequisites
- JDK/openjdk and JavaFX/openjfx SDK latest versions.
- ArcGIS Maps SDK for Qt.
- Qt 6.x.x latest, Qt Creator.
- Visual Studio 2022 with MSVC x64 toolchain (for Qt MSVC kits) / Xcode.
- Visual Studio Code (Java Extension Pack) or IntelliJ IDEA for JavaFX.
- ArcGIS Maps SDK for Qt.

## Build QtMapApp (Qt Creator)
1) Open `QtMapApp` in Qt Creator by using the CMakeLists.txt.
2) Select a Kit that matches your Qt install.
3) Modify the openjdk and ArcGIS Maps SDK version in the CMakeLists.txt.
4) Build the `QtMapApp` project, Note the build output directory; it must contain `QtMapApp.dll` (Windows) or `libQtMapApp.dylib` (macOS).

## Run JavaFxApp (VS Code)
1) Open the `JavaFxApp` in Visual studio code.
2) Ensure your JavaFX SDK, JDK, Qt6 and ArcGIS Qt SDK path is correct in the `.vscode/launch.json`.
   - Edit `.vscode/launch.json` - VM args, to adjust the paths for the `QtMapApp.dll` and openjfx installation.
   - Edit `.vscode/launch.json` - "env" - "Path", to adjust the paths for the Qt6 and ArcGIS_Maps_SDK_Qt installation.
3) Run the `JavaFxApp`. It loads the `QtMapApp.dll` and embeds the Qt map window inside the JavaFX stage.
4) Use the controls in the JavaFX app to interact with Qt Map app.

## Run JavaFxApp (IntelliJ IDEA)
To run JavaFxApp with IntelliJ IDEA, navigate to `Run` -> `Edit Configurations` and modify the `VM options` as specified below.<br>
Add these VM options (adjust paths):
```
-Djava.security.policy=java.policy
--module-path C:\tools\javafx-23.0.1\lib
--add-modules=javafx.controls,javafx.fxml,javafx.base,javafx.graphics
--add-opens javafx.graphics/javafx.stage=ALL-UNNAMED
--add-opens javafx.graphics/com.sun.glass.ui=ALL-UNNAMED
--add-exports=javafx.graphics/com.sun.javafx.tk=ALL-UNNAMED
--add-exports javafx.graphics/com.sun.javafx.tk.quantum=ALL-UNNAMED
-Djava.library.path=C:\applications\JavaFxToQt\QtMapApp\build\Desktop_Qt_6_10_0_MSVC2022_64bit-Release
```

