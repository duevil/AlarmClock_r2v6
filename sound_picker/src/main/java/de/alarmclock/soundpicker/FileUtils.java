package de.alarmclock.soundpicker;

import java.awt.Desktop;
import java.io.File;
import java.io.IOException;
import java.util.NoSuchElementException;
import java.util.regex.Pattern;

/**
 * Utility class providing file utils
 */
public final class FileUtils {
    private static final System.Logger LOGGER = System.getLogger("javafx");
    private static final Pattern EXTENSION = Pattern.compile("[.][^.]+$");

    /**
     * Private constructor as an instantiation is not necessary
     */
    private FileUtils() {
    }

    /**
     * Returns the name of a file with its extension filtered
     *
     * @param file The file whose name should be retrieved
     * @return {@link File#getName()} with the extension removed
     */
    public static String getNameWithoutExtension(File file) {
        return EXTENSION.matcher(file.getName()).replaceFirst("");
    }

    /**
     * Return only the extension of a file
     *
     * @param file The file whose extension should be retrieved
     * @return The extension part of {@link File#getName()}
     */
    public static String getExtension(File file) {
        var matcher = EXTENSION.matcher(file.getName());
        if (matcher.find()) return matcher.group();
        throw new NoSuchElementException("No extension found");
    }

    /**
     * Opens a file using {@link Desktop#open(File)}
     *
     * @param file The file to open
     */
    public static void open(File file) {
        try {
            Desktop.getDesktop().open(file);
        } catch (IOException e) {
            LOGGER.log(System.Logger.Level.WARNING, "File could not be opened", e);
        }
    }
}
