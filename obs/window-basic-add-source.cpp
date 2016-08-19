#include <QMessageBox>
#include "window-basic-main.hpp"
#include "window-basic-add-source.hpp"
#include "qt-wrappers.hpp"
#include "obs-app.hpp"

void OBSBasicAddSource::on_buttonBox_accepted()
{
	/*bool useExisting = ui->selectExisting->isChecked();
	bool visible = ui->sourceVisible->isChecked();

	if (useExisting) {
		QListWidgetItem *item = ui->sourceList->currentItem();
		if (!item)
			return;

		AddExisting(QT_TO_UTF8(item->text()), visible);
	} else {
		if (ui->sourceName->text().isEmpty()) {
			QMessageBox::information(this,
					QTStr("NoNameEntered.Title"),
					QTStr("NoNameEntered.Text"));
			return;
		}

		if (!AddNew(this, id, QT_TO_UTF8(ui->sourceName->text()),
					visible, newSource))
			return;
	}*/

	done(DialogCode::Accepted);
}

void OBSBasicAddSource::on_buttonBox_rejected()
{
	done(DialogCode::Rejected);
}

OBSBasicAddSource::OBSBasicAddSource(OBSBasic *parent)
	: QDialog (parent),
	  ui      (new Ui::OBSBasicAddSource)
{
	ui->setupUi(this);
}