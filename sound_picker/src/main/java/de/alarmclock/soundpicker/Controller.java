package de.alarmclock.soundpicker;

import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.Alert;
import javafx.scene.control.ListView;

/**
 * The main view's controller that combines the view with e new {@link Model}-instance
 */
public class Controller {
    private final Model model = new Model();
    @FXML
    public ListView<Sound> list;
    @FXML
    public Alert info;

    /**
     * Initializes the list view's items with the model's {@link Model#soundProperty() sounds}
     */
    @FXML
    public void initialize() {
        list.setItems(model.soundProperty());
    }

    /**
     * Action handler for {@link Model#addSounds() adding sound files}
     */
    @FXML
    public void addSounds() {
        model.addSounds();
    }

    /**
     * Action handler for {@link Model#createJson() creating the JSON file}
     */
    @FXML
    public void createJson() {
        model.createJson();
    }

    /**
     * Action handler for {@link Model#copySoundFiles() copying the sound files}
     */
    @FXML
    public void copyFiles() {
        model.copySoundFiles();
    }

    /**
     * Action handler for showing the application's information alert
     */
    @FXML
    public void showInfo() {
        info.show();
    }

    /**
     * Action handler for closing the application
     */
    @FXML
    public void close() {
        Platform.exit();
    }
}

