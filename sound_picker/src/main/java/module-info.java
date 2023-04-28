module de.alarmclock.soundpicker {
    requires com.fasterxml.jackson.databind;
    requires javafx.controls;
    requires javafx.fxml;
    requires java.desktop;

    opens de.alarmclock.soundpicker to javafx.fxml;
    exports de.alarmclock.soundpicker;
}
