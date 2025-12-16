package com.example.javafxqt;

import java.io.IOException;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.stage.Stage;

public class HelloApplication extends Application {
    @Override
    public void start(Stage stage) throws IOException {
        FXMLLoader fxmlLoader = new FXMLLoader(HelloApplication.class.getResource("hello-view.fxml"));
        Scene scene = new Scene(fxmlLoader.load(), 1200, 900);
        stage.setTitle("JavaFx Hello Application");
        stage.setScene(scene);
        stage.show();

        // Add a shutdown hook to call closeQtApplication() when the application is closed
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            HelloController controller = fxmlLoader.getController();
            controller.closeQtApplication();
        }));
    }

    public static void main(String[] args) {
       launch(args);
    }
}