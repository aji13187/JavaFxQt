# JavaFxToQt - Migration options from JavaFX to Qt.<br>

The demo code includes two applications: JavaFxApp and QtMapApp.<br>
These applications can be build using QtCreator for the Qt application and IntelliJ IDEA (or any Java IDE) for the JavaFx application.<br>\
Execute the JavaFxApp to demonstrate the embedding of the Qt Map application within JavaFx.<br>
The demo application also features some basic interactions between the JavaFx and Qt Map applications using JNI.<br>\
This code will work on both Windows and MacOS, Make sure to configure the JDK, JAVAFX, QT and ArcGIS SDK paths in both Qt and JavaFx IDE's based on the platform.<br>

**Configuration:**
In IntelliJ IDEA, navigate to `Run` -> `Edit Configurations` and modify the `VM options` as specified below.<br>
If you are using Visual Studio Code as IDE, make sure to update the `launch.json` file accordingly.<br>
```
-Djava.security.policy=java.policy
--module-path C:\\Users\\aji13187\\Downloads\\openjfx-23.0.1\\javafx-sdk-23.0.1\\lib
--add-modules=javafx.controls,javafx.fxml,javafx.base,javafx.graphics
--add-opens javafx.graphics/javafx.stage=ALL-UNNAMED
--add-opens javafx.graphics/com.sun.glass.ui=ALL-UNNAMED
--add-exports=javafx.graphics/com.sun.javafx.tk=ALL-UNNAMED
--add-exports javafx.graphics/com.sun.javafx.tk.quantum=ALL-UNNAMED
-Djava.library.path=C:\\applications\qt\\JavaFxToQt\\QtMapApp\\build\\Qt_6_5_6_msvc2019_64-Release;
```
Additionally, configure the environment variables in the same window for Qt and the ArcGIS SDK libraries.<br>
(Path will change depending on the platform, eg: use DYLD_LIBRARY_PATH on macOS).<br>
![image](https://devtopia.esri.com/aji13187/JavaFxToQt/assets/6964/6973f9b3-33be-4305-95e5-579e86fa914c)

