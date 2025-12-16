package com.example.javafxqt;

import java.lang.reflect.Method;

import com.sun.javafx.tk.TKStage;
import com.sun.jna.Pointer;

import javafx.fxml.FXML;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

public class HelloController {
    static {
        try {
            System.loadLibrary("QtMapApp");
        } catch (UnsatisfiedLinkError e) {
            e.printStackTrace();
        }
    }

    @FXML private ComboBox<String> comboBoxViewpoint;
    public boolean isQtAppLoaded = false;
    public double rotationValue = 45.0;

    public native void runQtMapOnJavaFxWindow(long aLong);
    public native void setViewPoint(double x, double y);
    public native void setViewPointWithScale(double x, double y, double scaleValue);
    public native void setViewPointRotation(double rotation);
    public native void setViewPointScale(double scale);
    public native void closeQtApplication();

    public void callJavaFxMethodFromQt(double x, double y) {
        xLabel.setText(String.valueOf(x));
        yLabel.setText(String.valueOf(y));
    }

    @FXML private VBox container;
    @FXML private Label xLabel;
    @FXML private Label yLabel;
    @FXML private Label versionLabel;

    @FXML
    protected void onHelloButtonClick() {
        launchQtApplication();
    }

    @FXML
    public void initialize() {
        comboBoxViewpoint.setOnAction(event -> {
            String selectedItem = comboBoxViewpoint.getSelectionModel().getSelectedItem();
            changeViewpoint(selectedItem);
        });

        String javafxVersion = System.getProperty("javafx.version");
        String jdkVersion = System.getProperty("java.version");
        versionLabel.setText("JavaFX Version: " + javafxVersion + ", JDK Version: " + jdkVersion);
        comboBoxViewpoint.getSelectionModel().selectFirst();
    }

    private void changeViewpoint(String viewpoint) {
        if (!isQtAppLoaded) {
            System.out.println("Qt Application is not loaded yet");
            return;
        }
        switch (viewpoint) {
            case "Center" -> setViewPoint(-117.195681, 34.056218);
            case "Center and scale" -> setViewPointWithScale(-157.564, 20.677, 40000000.0);
            case "Rotation" -> {
                rotationValue = (rotationValue + 45.0) % 360;
                setViewPointRotation(rotationValue);
            }
            case "Scale 1:5,000,000" -> setViewPointScale(5000000.0);
            case "Scale 1:10,000,000" -> setViewPointScale(10000000.0);
            default -> System.out.println("Unknown viewpoint: " + viewpoint);
        }
    }

    @FXML
    private void onZoomToPointClick() {
        if (!isQtAppLoaded) {
            System.out.println("Qt Application is not loaded yet");
            return;
        }
        if (xLabel.getText().isEmpty() || yLabel.getText().isEmpty()) {
            System.out.println("Please click on the map to get the coordinates");
            return;
        }
        double x = Double.parseDouble(xLabel.getText());
        double y = Double.parseDouble(yLabel.getText());
        setViewPointWithScale(x, y, 400000.0);
    }

    void launchQtApplication() {
        try {
            Stage stage = (Stage) container.getScene().getWindow();
            long parentHwnd = getWindowId(stage);
            if (!isQtAppLoaded) {
                System.out.println("Launching Qt Application with window Id = " + parentHwnd);
                isQtAppLoaded = true;
                runQtMapOnJavaFxWindow(parentHwnd);
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static long getWindowId(Stage stage) {
        try {
            Method tkStageGetter;
            try {
                tkStageGetter = stage.getClass().getSuperclass().getDeclaredMethod("getPeer");
            } catch (NoSuchMethodException ex) {
                tkStageGetter = stage.getClass().getMethod("impl_getPeer");
            }
            tkStageGetter.setAccessible(true);
            TKStage tkStage = (TKStage) tkStageGetter.invoke(stage);

            if (tkStage == null) {
                throw new NullPointerException("TKStage is null");
            }

            Method getPlatformWindow = tkStage.getClass().getDeclaredMethod("getPlatformWindow");
            getPlatformWindow.setAccessible(true);
            Object platformWindow = getPlatformWindow.invoke(tkStage);
            if (System.getProperty("os.name").toLowerCase().contains("mac")) {
                Pointer nsView = getNSView(platformWindow);
                return Pointer.nativeValue(nsView);
            } else {
                Method getNativeHandle = platformWindow.getClass().getMethod("getNativeHandle");
                getNativeHandle.setAccessible(true);
                Object nativeHandle = getNativeHandle.invoke(platformWindow);
                return (long) nativeHandle;
            }
        } catch (Throwable e) {
            System.err.println("Error getting Window Pointer");
            e.printStackTrace();
            return 0;
        }
    }

    private static Pointer getNSView(Object platformWindow) {
        try {
            CocoaLibrary cocoa = CocoaLibrary.INSTANCE;
            Pointer nsApplicationClass = cocoa.objc_getClass("NSApplication");
            Pointer sharedApplicationSelector = cocoa.sel_registerName("sharedApplication");
            Pointer keyWindowSelector = cocoa.sel_registerName("keyWindow");
            Pointer contentViewSelector = cocoa.sel_registerName("contentView");

            Pointer sharedApplication = cocoa.objc_msgSend(nsApplicationClass, sharedApplicationSelector);

            Pointer keyWindow = cocoa.objc_msgSend(sharedApplication, keyWindowSelector);

            if (keyWindow != null) {
                return cocoa.objc_msgSend(keyWindow, contentViewSelector);
            } else {
                System.err.println("No key window found");
                return null;
            }
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }
}