package de.alarmclock.soundpicker;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.control.ButtonBar;
import javafx.scene.control.ButtonType;
import javafx.scene.control.ChoiceDialog;
import javafx.stage.Stage;

import java.util.Locale;
import java.util.Map;
import java.util.ResourceBundle;

/**
 * The main {@link Application}
 */
public class App extends Application {

    /**
     * Shows a {@link ChoiceDialog} for choosing the applications language
     * and sets the {@link Locale#setDefault(Locale) default Locale}
     *
     * @return The chosen {@link Locale} or being {@link Locale#ENGLISH} if none was chosen
     */
    private static Locale chooseLocal() {
        record Choice(Locale locale, String s) {
            @Override
            public String toString() {
                return s;
            }
        }
        var dialog = new ChoiceDialog<>(null,
                new Choice(Locale.ENGLISH, "English"),
                new Choice(Locale.GERMAN, "Deutsch"));
        dialog.setTitle("Choose language");
        dialog.setHeaderText("Choose the\napplication's language");
        dialog.setGraphic(null);
        var buttons = dialog.getDialogPane().getButtonTypes();
        buttons.clear();
        buttons.addAll(ButtonType.OK, new ButtonType("Default", ButtonBar.ButtonData.CANCEL_CLOSE));
        return dialog.showAndWait().map(Choice::locale).orElse(Locale.ENGLISH);
    }

    /**
     * Loads the main views FXML and starts the JavaFX application
     *
     * @param primaryStage the primary stage for this application, onto which
     *                     the application scene can be set.
     *                     Applications may create other stages, if needed, but they will not be
     *                     primary stages.
     * @throws Exception When an exception occurs during the loading of the FXML file
     */
    @Override
    public void start(Stage primaryStage) throws Exception {
        setLocal();
        var view = ClassLoader.getSystemResource("mainView.fxml");
        if (view == null) throw new IllegalStateException("View is null");
        var viewResources = ResourceBundle.getBundle("view");
        if (viewResources == null) throw new IllegalStateException("Properties is null");
        var loader = new FXMLLoader(view, viewResources);
        var scene = new Scene(loader.load());
        primaryStage.setScene(scene);
        primaryStage.setTitle(viewResources.getString("title"));
        primaryStage.setResizable(false);
        primaryStage.show();
    }

    /**
     * Reads the {@link Application#getParameters() Application's parameter},
     * searches for a language parameter and sets the {@link Locale#setDefault(Locale) default Locale}
     * if the parameter language is equal to either {@link Locale#ENGLISH} or {@link Locale#GERMAN},
     * or calls {@link App#chooseLocal()} and sets it to the value returned from that call if no parameter was found
     * <p>
     * <b>Info:</b><br>The language parameter must be off the form:
     * <ul style="list-style-type: none">
     *     <li><code>--l=[language]</code></li>
     *     <li><code>--lang=[language]</code></li>
     *     <li><code>--language=[language]</code></li>
     * </ul>
     * <p>
     *  For example: <code>--lang=de</code>
     */
    private void setLocal() {
        this.getParameters().getNamed().entrySet().stream().filter((Map.Entry<String, String> arg) -> {
            String key = arg.getKey();
            return "l".equals(key) || "lang".equals(key) || "language".equals(key);
        }).findFirst().map(Map.Entry::getValue).map(Locale::forLanguageTag).filter((Locale locale) -> {
            var langStr = locale.getLanguage();
            return Locale.ENGLISH.getLanguage().equals(langStr) || Locale.GERMAN.getLanguage().equals(langStr);
        }).ifPresentOrElse(Locale::setDefault, () -> {
            var currentLangStr = Locale.getDefault().getLanguage();
            if (!Locale.ENGLISH.getLanguage().equals(currentLangStr) &&
                !Locale.GERMAN.getLanguage().equals(currentLangStr)) Locale.setDefault(chooseLocal());
        });
    }
}
