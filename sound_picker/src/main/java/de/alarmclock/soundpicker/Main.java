package de.alarmclock.soundpicker;

import javafx.application.Application;

/**
 * Main class for launching the application
 */
public class Main {

    /**
     * Calls {@link Application#launch(Class, String...)} for the {@link App}
     *
     * @param args The runtime arguments that will be passed to the launch method
     * @apiNote This is needed, as why ever launching the application directly through the application class
     * will cause a missing JavaFX runtime error
     */
    public static void main(String[] args) {
        Application.launch(App.class, args);
    }
}
