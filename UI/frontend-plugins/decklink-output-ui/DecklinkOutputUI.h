#pragma once

#include <QDialog>

#include "ui_output.h"
#include "../../UI/properties-view.hpp"

class DecklinkOutputUI : public QDialog {
Q_OBJECT
private:
	OBSPropertiesView *propertiesView;

public slots:
	void StartOutput();
	void StopOutput();

public:
	std::unique_ptr<Ui_Output> ui;
	DecklinkOutputUI(QWidget *parent);

	void ShowHideDialog();
	void SetupPropertiesView();
	void SaveSettings();
};
