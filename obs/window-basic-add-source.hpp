#pragma once

#include <obs.hpp>
#include <memory>

#include "ui_OBSBasicAddSource.h"

class OBSBasic;

class OBSBasicAddSource : public QDialog {
	Q_OBJECT

private:
	std::unique_ptr<Ui::OBSBasicAddSource> ui;

private slots:
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();

public:
	OBSBasicAddSource(OBSBasic *parent);
};