package de.alarmclock.soundpicker;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectWriter;
import javafx.beans.property.ListProperty;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleListProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.collections.FXCollections;
import javafx.concurrent.Service;
import javafx.concurrent.Task;
import javafx.concurrent.WorkerStateEvent;
import javafx.scene.control.ButtonType;
import javafx.stage.DirectoryChooser;
import javafx.stage.FileChooser;

import java.io.File;
import java.io.IOException;
import java.io.UncheckedIOException;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.util.ResourceBundle;

/**
 * The application model; handles the {@link Sound Sounds}
 */
public class Model {
    private static final FileChooser SOUND_PICKER = new FileChooser();
    private static final FileChooser JSON_SAVER = new FileChooser();
    private static final DirectoryChooser COPY_LOCATION_CHOOSER = new DirectoryChooser();
    private static final ObjectWriter JSON_WRITER = new ObjectMapper().writerWithDefaultPrettyPrinter();
    private static final ObjectProperty<File> INITIAL_DIRECTORY_PROPERTY = new SimpleObjectProperty<>();

    static {
        var view = ResourceBundle.getBundle("view");
        if (view == null) throw new IllegalStateException("Properties is null");

        SOUND_PICKER.setTitle(view.getString("fileChooser.pickSounds.title"));
        String soundType = view.getString("fileChooser.pickSounds.type");
        var soundFilter = new FileChooser.ExtensionFilter(soundType, "*.mp3");
        SOUND_PICKER.getExtensionFilters().add(soundFilter);
        SOUND_PICKER.initialDirectoryProperty().bind(INITIAL_DIRECTORY_PROPERTY);

        JSON_SAVER.setTitle(view.getString("fileChooser.saveJson.title"));
        String jsonType = view.getString("fileChooser.saveJson.type");
        var jsonFilter = new FileChooser.ExtensionFilter(jsonType, "*.json", "*.txt");
        JSON_SAVER.getExtensionFilters().add(jsonFilter);
        JSON_SAVER.initialDirectoryProperty().bind(INITIAL_DIRECTORY_PROPERTY);

        COPY_LOCATION_CHOOSER.setTitle(view.getString("fileChooser.copyFiles.title"));
        COPY_LOCATION_CHOOSER.initialDirectoryProperty().bind(INITIAL_DIRECTORY_PROPERTY);

        INITIAL_DIRECTORY_PROPERTY.set(new File("."));
    }

    private final ListProperty<Sound> sounds = new SimpleListProperty<>(FXCollections.observableArrayList());

    /**
     * Creates a {@link Task} for renaming and copying sound files
     *
     * @param destination The directory the sound files will be copied to
     * @param sounds      The list of {@link Sound Sounds} which should be copied
     * @return The created Task
     */
    private static Task<Void> createCopyTask(File destination, final ListProperty<Sound> sounds) {
        return new Task<>() {
            @Override
            protected Void call() throws Exception {
                for (Sound sound : sounds) {
                    var name = "%04d%s".formatted(sound.getNumber(), FileUtils.getExtension(sound.getFile()));
                    Files.copy(
                            sound.getFile().toPath(),
                            destination.toPath().resolve(name),
                            StandardCopyOption.REPLACE_EXISTING
                    );
                    updateProgress(sound.getNumber(), sounds.size());
                }
                return null;
            }
        };
    }

    /**
     * Shows a {@link FileChooser}-dialog fo picking the sound files to load
     * and adds the picked files as {@link Sound Sounds} to the model's sound property
     */
    public void addSounds() {
        var files = SOUND_PICKER.showOpenMultipleDialog(null);
        if (files != null) {
            INITIAL_DIRECTORY_PROPERTY.set(files.get(0).getParentFile());
            for (var file : files) Sound.createAndAddToList(file, sounds);
        }
    }

    /**
     * Shows a {@link FileChooser}-dialog for choosing the JSON file's destination directory,
     * creates the JSON file out of the currently loaded {@link Sound Sounds}
     * and {@link FileUtils#open(File) opens} the file afterwards
     */
    public void createJson() {
        var destination = JSON_SAVER.showSaveDialog(null);
        if (destination != null) {
            INITIAL_DIRECTORY_PROPERTY.set(destination.getParentFile());
            try {
                JSON_WRITER.writeValue(destination, sounds);
            } catch (IOException e) {
                throw new UncheckedIOException(e);
            }
            FileUtils.open(destination);
        }
    }

    /**
     * Shows a {@link FileChooser}-dialog for choosing the destination directory
     * and copies and especially renames the currently loaded {@link Sound Sounds} into the chosen directory,
     * which will be {@link FileUtils#open(File) opened} afterwards
     * <p>
     * During copying a {@link CopyingAlert} with the progress of the task will be shown
     */
    public void copySoundFiles() {
        var destination = COPY_LOCATION_CHOOSER.showDialog(null);
        if (destination != null) {
            INITIAL_DIRECTORY_PROPERTY.set(destination);
            var alert = new CopyingAlert();
            Service<Void> service = new Service<>() {
                @Override
                protected Task<Void> createTask() {
                    var task = createCopyTask(destination, sounds);
                    alert.forWorker(task);
                    return task;
                }
            };
            service.setOnSucceeded((WorkerStateEvent event) -> {
                alert.setResult(ButtonType.FINISH);
                alert.close();
            });
            service.start();
            if (!alert.showAndWaitOrCancel() && service.isRunning()) service.cancel();
            FileUtils.open(destination);
        }
    }

    /**
     * A {@link ListProperty} holding the currently loaded {@link Sound Sounds}
     *
     * @return The models sound property
     */
    public ListProperty<Sound> soundProperty() {
        return sounds;
    }
}
