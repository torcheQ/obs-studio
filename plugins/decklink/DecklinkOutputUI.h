#pragma once

#include <QDialog>

#include "ui_output.h"

class DecklinkOutputUI : public QDialog {

public:
    std::unique_ptr<Ui_Output> ui;
    DecklinkOutputUI(QWidget *parent);

    void ShowHideDialog();
};
