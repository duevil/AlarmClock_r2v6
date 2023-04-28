package de.alarmclock.soundpicker;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.scene.control.MultipleSelectionModel;

/**
 * An implementation of a {@link MultipleSelectionModel} for preventing a selection in any way,
 * making the element using this selection model effectively unselectable
 *
 * @param <T> The selection models type
 * @author <a href="https://stackoverflow.com/a/46186195">Mordechai on StackOverflow</a>
 * @implNote All methods either return a default value or are simply doing nothing
 */
@SuppressWarnings({"UnnecessaryReturnStatement", "java:S3626", "EmptyMethod"}) // return used to show the no-action
public class NoSelectionModel<T> extends MultipleSelectionModel<T> {

    @Override
    public ObservableList<Integer> getSelectedIndices() {
        return FXCollections.emptyObservableList();
    }

    @Override
    public ObservableList<T> getSelectedItems() {
        return FXCollections.emptyObservableList();
    }

    @Override
    public void selectIndices(int index, int... indices) {
        return;
    }

    @Override
    public void selectAll() {
        return;
    }

    @Override
    public void selectFirst() {
        return;
    }

    @Override
    public void selectLast() {
        return;
    }

    @Override
    public void clearAndSelect(int index) {
        return;
    }

    @Override
    public void select(int index) {
        return;
    }

    @Override
    public void select(T obj) {
        return;
    }

    @Override
    public void clearSelection(int index) {
        return;
    }

    @Override
    public void clearSelection() {
        return;
    }

    @Override
    public boolean isSelected(int index) {
        return false;
    }

    @Override
    public boolean isEmpty() {
        return true;
    }

    @Override
    public void selectPrevious() {
        return;
    }

    @Override
    public void selectNext() {
        return;
    }
}
