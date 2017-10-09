#include "DecklinkOutputUI.h"

DecklinkOutputUI::DecklinkOutputUI(QWidget *parent)
        : QDialog(parent),
          ui(new Ui_Output)
{
    ui->setupUi(this);
}

void DecklinkOutputUI::ShowHideDialog()
{
    if (!isVisible()) {
        setVisible(true);
    } else {
        setVisible(false);
    }
}