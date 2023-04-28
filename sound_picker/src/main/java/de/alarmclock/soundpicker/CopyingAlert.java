package de.alarmclock.soundpicker;

import javafx.concurrent.Worker;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressBar;
import javafx.util.converter.PercentageStringConverter;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.util.ResourceBundle;

/**
 * Implements an alert showing a {@link ProgressBar} for displaying the progress of file copying
 */
public class CopyingAlert extends Alert {
    @FXML
    public ProgressBar progressBar;
    @FXML
    public Label progressLabel;
    @FXML
    public Alert cancelConfirm;

    /**
     * Constructor; {@link FXMLLoader#load() loads} the elements FXML,
     * sets the own instance as {@link FXMLLoader#setRoot(Object) root}
     * and {@link FXMLLoader#setController(Object) controller}
     * and initializes property bindings
     */
    public CopyingAlert() {
        super(AlertType.INFORMATION);
        var fxml = ClassLoader.getSystemResource("copyingAlert.fxml");
        var resources = ResourceBundle.getBundle("view");
        var loader = new FXMLLoader(fxml, resources);
        loader.setRoot(this);
        loader.setController(this);
        try {
            loader.load();
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
        progressLabel.textProperty().bindBidirectional(progressBar.progressProperty(), new PercentageStringConverter());
        getDialogPane().lookupButton(ButtonType.FINISH).setVisible(false);
    }

    /**
     * Bind the alert's {@link ProgressBar#progressProperty()} to the given {@link Worker#progressProperty()}
     *
     * @param worker The worker this alert's progress should be bound to
     */
    public void forWorker(Worker<?> worker) {
        progressBar.progressProperty().bind(worker.progressProperty());
    }

    /**
     * Calls {@link Alert#showAndWait()} and if Cancel was pressed, asks for confirmation and bevor canceling,
     * recursively until the alert was either closed manually or cancellation was confirmed
     *
     * @return true if the alert's finish button was pressed or false if cancellation was confirmed
     */
    public boolean showAndWaitOrCancel() {
        return showAndWait().filter(t -> t == ButtonType.FINISH).isPresent() ||
               (cancelConfirm.showAndWait().filter(t -> t == ButtonType.OK).isEmpty() && showAndWaitOrCancel());
    }
}
