package de.alarmclock.soundpicker;

import com.fasterxml.jackson.annotation.JsonFormat;
import com.fasterxml.jackson.annotation.JsonGetter;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import javafx.beans.binding.Bindings;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.IntegerProperty;
import javafx.beans.property.ReadOnlyIntegerProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.collections.ObservableList;

import java.io.File;

/**
 * The application's main data class storing a sound file, a number assigned to this sound, its name
 * and whether the sound might be eligible for random playback
 */
@JsonPropertyOrder({"number", "name", "allowRandom"})
@JsonIgnoreProperties({"file"})
public final class Sound {
    private static final int MAX_NAME_LENGTH = 16;
    private final File file;
    private final IntegerProperty number = new SimpleIntegerProperty(this, "sound number");
    private final StringProperty name = new SimpleStringProperty(this, "sound name");
    private final BooleanProperty allowRandom = new SimpleBooleanProperty(this, "sound allow random", true);

    /**
     * Constructor; stores a sound file,
     * initializes the name with the file's {@link FileUtils#getNameWithoutExtension(File) name}
     *
     * @param file The sounds underlying file
     */
    private Sound(File file) {
        this.file = file;
        String nameWithoutExtension = FileUtils.getNameWithoutExtension(file);
        name.set(nameWithoutExtension.substring(0, Math.min(nameWithoutExtension.length(), MAX_NAME_LENGTH)).trim());
    }

    /**
     * {@link Sound#Sound Creates} a new sound from the given file,
     * binds its number to its index in the given sound list and adds it to the list
     *
     * @param soundFile The file with which an instance will be created
     * @param soundList A {@link ObservableList} to which the sound will be added and the sounds number will be bind to
     */
    public static void createAndAddToList(File soundFile, ObservableList<Sound> soundList) {
        var sound = new Sound(soundFile);
        sound.number.bind(Bindings.createIntegerBinding(() -> soundList.indexOf(sound), soundList).add(1));
        soundList.add(sound);
    }

    /**
     * Returns the current value of {@link Sound#numberProperty()}
     *
     * @return The current value of {@link Sound#numberProperty()}
     */
    @JsonGetter("number")
    public int getNumber() {
        return number.get();
    }

    /**
     * The property of the number this sound is assigned to
     *
     * @return A {@link ReadOnlyIntegerProperty} containing the bound number of this sound
     */
    public ReadOnlyIntegerProperty numberProperty() {
        return number;
    }

    /**
     * Returns the current value of {@link Sound#numberProperty()}
     *
     * @return The current value of {@link Sound#numberProperty()}
     */
    @JsonGetter("name")
    public String getName() {
        return name.get();
    }

    /**
     * The property of this sound's name
     *
     * @return A {@link StringProperty} containing the name of this sound
     */
    public StringProperty nameProperty() {
        return name;
    }

    /**
     * Returns the current value of {@link Sound#allowRandomProperty()}
     *
     * @return The current value of {@link Sound#allowRandomProperty()}
     */
    @JsonGetter("allowRandom")
    @JsonFormat(shape = JsonFormat.Shape.NUMBER)
    public boolean isAllowRandom() {
        return allowRandom.get();
    }

    /**
     * The property of this sound denoting whether this sound is eligible for random playback
     *
     * @return A {@link BooleanProperty} containing the allowRandom-value of this sound
     */
    public BooleanProperty allowRandomProperty() {
        return allowRandom;
    }

    /**
     * Returns the underlying sound file of this sound
     *
     * @return The underlying sound file of this sound
     */
    public File getFile() {
        return file;
    }
}
