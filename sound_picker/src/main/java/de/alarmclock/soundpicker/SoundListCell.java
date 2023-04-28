package de.alarmclock.soundpicker;

import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.control.CheckBox;
import javafx.scene.control.CheckMenuItem;
import javafx.scene.control.Label;
import javafx.scene.control.ListCell;
import javafx.scene.control.ListView;
import javafx.scene.control.TextField;
import javafx.scene.control.TextFormatter;
import javafx.util.Callback;

import java.io.File;
import java.io.IOException;
import java.io.UncheckedIOException;
import java.util.Collections;
import java.util.Optional;
import java.util.ResourceBundle;
import java.util.regex.Pattern;

/**
 * A custom {@link ListCell} for displaying and editing a {@link Sound}
 */
public class SoundListCell extends ListCell<Sound> {
    @FXML
    public Label number;
    @FXML
    public TextField name;
    @FXML
    public CheckMenuItem allowRandomA;
    @FXML
    public CheckBox allowRandomB;
    @FXML
    public CheckMenuItem allowRandomC;

    /**
     * Constructor; {@link FXMLLoader#load() loads} the elements FXML,
     * sets the own instance as {@link FXMLLoader#setRoot(Object) root}
     * and {@link FXMLLoader#setController(Object) controller}
     * and initializes property bindings
     */
    public SoundListCell() {
        var fxml = ClassLoader.getSystemResource("soundListCell.fxml");
        var resources = ResourceBundle.getBundle("view");
        var loader = new FXMLLoader(fxml, resources);
        loader.setRoot(this);
        loader.setController(this);
        try {
            loader.load();
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }

        getGraphic().visibleProperty().bind(emptyProperty().not());
        allowRandomA.selectedProperty().bindBidirectional(allowRandomB.selectedProperty());
        allowRandomA.selectedProperty().bindBidirectional(allowRandomC.selectedProperty());
    }

    /**
     * Updates the cell's display elements' bindings to {@link Sound} properties;
     * removes all previse bindings and if the cell is not empty creates new bindings with the new Sound properties
     *
     * @param item  The new item for the cell.
     * @param empty Whether this cell represents data from the list. If it
     *              is empty, then it does not represent any domain data, but is a cell
     *              being used to render an "empty" row.
     */
    @Override
    protected void updateItem(Sound item, boolean empty) {
        Optional.ofNullable(getItem()).ifPresent((Sound sound) -> {
            name.textProperty().unbindBidirectional(sound.nameProperty());
            allowRandomA.selectedProperty().unbindBidirectional(sound.allowRandomProperty());
        });
        super.updateItem(item, empty);
        if (empty) {
            number.textProperty().unbind();
        } else {
            number.textProperty().bind(item.numberProperty().asString("#%04d"));
            name.textProperty().bindBidirectional(item.nameProperty());
            allowRandomA.selectedProperty().bindBidirectional(item.allowRandomProperty());
        }
    }

    /**
     * Action handler for swapping this cell's {@link Sound} with the sound above it
     */
    @FXML
    void swapAbove() {
        Optional.ofNullable(getItem()).ifPresent((Sound sound) -> {
            int soundNumber = sound.getNumber() - 1;
            Collections.swap(getListView().getItems(), Math.max(0, soundNumber - 1), soundNumber);
        });
    }

    /**
     * Action handler for swapping this cell's {@link Sound} with the sound below it
     */
    @FXML
    void swapBelow() {
        Optional.ofNullable(getItem()).ifPresent((Sound sound) -> {
            int soundNumber = sound.getNumber() - 1;
            var sounds = getListView().getItems();
            Collections.swap(sounds, soundNumber, Math.min(sounds.size() - 1, soundNumber + 1));
        });
    }

    /**
     * {@link FileUtils#open(File) Opens} the cell's {@link Sound#getFile() sound file}
     */
    @FXML
    void play() {
        Optional.ofNullable(getItem()).map(Sound::getFile).ifPresent(FileUtils::open);
    }

    /**
     * Remove the cell's sound from the {@link ListView#getItems() ListView's items} this cell is contained in
     */
    @FXML
    void remove() {
        Optional.ofNullable(getItem()).ifPresent(getListView().getItems()::remove);
    }

    /**
     * Toggles the {@link Sound#allowRandomProperty()} of this cell's {@link Sound}
     */
    @FXML
    void toggleAllowRandom() {
        allowRandomA.setSelected(!allowRandomA.isSelected());
    }

    /**
     * Implementation of a {@link Callback} for the {@link ListView#cellFactoryProperty()}
     * creating a new instance of a {@link SoundListCell}
     */
    @SuppressWarnings("rawtypes") // raw types to be able to use this in a fxml file
    public static class Factory implements Callback<ListView, ListCell> {
        @Override
        public ListCell call(ListView param) {
            return new SoundListCell();
        }
    }

    /**
     * Implements a simple {@link TextFormatter} for filtering an input that must meet certain criteria:
     * <ul>
     *     <li>The input must be at most 16 characters long</li>
     *     <li>The input must not have any leading whitespaces</li>
     *     <li>The input must not have a whitespace as the 16th character</li>
     * </ul>
     */
    public static class NoEmptyTextFormatter extends TextFormatter<Object> {
        private static final Pattern PATTERN = Pattern.compile("^\\S.{0,14}\\S?$", Pattern.UNICODE_CHARACTER_CLASS);

        public NoEmptyTextFormatter() {
            super(c -> PATTERN.matcher(c.getControlNewText()).matches() ? c : null);
        }
    }
}
