#include <mainwindow.hpp>
#include "ui_mainwindow.h"
#include <QVariant>
#include <QFileDialog>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QColorDialog>
#include <QPushButton>
#include <QList>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
	connect(ui->viewport->camera(), &Camera::moved, [=] (const QVector3D& position) {
		ui->camPos->setText(QString("X: %1, Y: %2, Z: %3").arg(position.x()).arg(position.y()).arg(position.z()));
	});
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::on_viewport_childAdded(QObject* obj) {
	auto item = new QTreeWidgetItem(ui->actorList);
	item->setText(0, obj->objectName());

	auto ptr = QVariant::fromValue(obj);
	item->setData(0, Qt::UserRole, ptr);

	connect(obj, &QObject::objectNameChanged, [=](QString newName) {
		item->setText(0, newName);
	});

	connect(obj, &QObject::destroyed, [=]() {
		ui->actorList->removeItemWidget(item, 0);
		delete item;
	});

	ui->actorList->addTopLevelItem(item);
}

QQuaternion fromEuler(const QVector3D& euler) {
	double c1 = cos(euler.x() / 2);
	double s1 = sin(euler.x() / 2);
	double c2 = cos(euler.y() / 2);
	double s2 = sin(euler.y() / 2);
	double c3 = cos(euler.z() / 2);
	double s3 = sin(euler.z() / 2);
	double c1c2 = c1 * c2;
	double s1s2 = s1 * s2;
	auto w = c1c2 * c3 - s1s2 * s3;
	auto x = c1c2 * s3 + s1s2 * c3;
	auto y = s1 * c2 * c3 + c1 * s2 * s3;
	auto z = c1 * s2 * c3 - s1 * c2 * s3;
	auto angle = 2 * acos(w);
	double norm = x * x + y * y + z * z;
	if (norm < 0.001) {
		x = 1;
		y = z = 0;
	} else {
		norm = sqrt(norm);
		x /= norm;
		y /= norm;
		z /= norm;
	}
	return QQuaternion(angle, x, y, z);
}

QWidget* MainWindow::widgetForVariant(QTreeWidgetItem* line, VarGetter get, VarSetter set) {
	auto prop = get();
	switch (prop.type()) {
		case QMetaType::QVector3D: {
			void (QDoubleSpinBox::*changeSignal)(double) = &QDoubleSpinBox::valueChanged;
			auto value = qvariant_cast<QVector3D>(prop);
			auto vector = new QVector3D(value);

			auto container = new QWidget;
			auto hbox = new QHBoxLayout;
			hbox->setMargin(1);
			container->setLayout(hbox);

			auto spinners = new QDoubleSpinBox* [3];
			for (int i = 0; i < 3; i++) {
				spinners[i] = new QDoubleSpinBox(container);
				spinners[i]->setRange(-2147483647, 2147483647);
				switch (i) {
					case 0:
						spinners[i]->setValue(vector->x());
						break;
					case 1:
						spinners[i]->setValue(vector->y());
						break;
					case 2:
						spinners[i]->setValue(vector->z());
						break;
				}
				hbox->addWidget(spinners[i]);
				connect(spinners[i], changeSignal, [=](double value) {
					switch (i) {
						case 0:
							vector->setX(value);
							break;
						case 1:
							vector->setY(value);
							break;
						case 2:
							vector->setZ(value);
							break;
					}
					set(*vector);
				});
			}

			return container;
		}
		case QMetaType::QString: {
			auto textBox = new QLineEdit;
			textBox->setText(prop.toString());
			connect(textBox, &QLineEdit::textEdited, set);
			return textBox;
		}
		case QMetaType::QColor: {
			auto colBtn = new QPushButton;

			/*auto value = qvariant_cast<QColor>(prop);
			auto col = const_cast<QColor*>(&value);
			colBtn->setText(QString(col->value()));*/

			connect(colBtn, &QPushButton::clicked, [=] () {
				auto value = qvariant_cast<QColor>(get());
				auto col = QColorDialog::getColor(value, nullptr, tr("Face color"));
				set(col);
				//colBtn->setText(QString("rgb(%1, %2, %3)").arg(col.red()).arg(col.green()).arg(col.blue()));
				//colBtn->palette().setColor(QPalette::Background, col);
				colBtn->setStyleSheet(QString("background-color: %1").arg(col.name()));
			});

			return colBtn;
		}
		case QMetaType::QVariantList: {
			auto value = prop.toList();
			auto list = new QVariantList(value);

			for (int i = 0; i < list->size(); i++) {
				auto item = new QTreeWidgetItem;
				item->setText(0, QString("Item %1").arg(i));
				line->addChild(item);

				auto widget = widgetForVariant(item, [=] () {
					return list->at(i);
				}, [=] (const QVariant& val) {
					list->operator[](i) = val;
				});

				line->treeWidget()->setItemWidget(item, 1, widget);
			}

			break;
		}
		case QMetaType::QQuaternion: {
			//TODO
			break;
		}
		default:
			qDebug() << prop.type();
			return new QLabel(prop.toString());
	}

	return nullptr;
}

void MainWindow::on_actorList_currentItemChanged(QTreeWidgetItem *current)
{
	auto ptr = current->data(0, Qt::UserRole);
	if (ptr.isValid() && static_cast<QMetaType::Type>(ptr.type()) == QMetaType::QObjectStar) {
		QObject* obj = qvariant_cast<QObject*>(ptr);
		if (obj) {
			auto info = ui->infoWidget;
			auto metaObject = obj->metaObject();
			auto count = metaObject->propertyCount();
			info->clear();
			for (int i = 0; i < count; i++) {
				auto prop = metaObject->property(i).name();
				auto line = new QTreeWidgetItem;
				line->setText(0, prop);
				info->addTopLevelItem(line);
				info->setItemWidget(line, 1, widgetForVariant(line, [=] () {
					return obj->property(prop);
				}, [=] (const QVariant& val) {
					obj->setProperty(prop, val);
				}));
			}
		}
	}
}

void MainWindow::on_actionOpen_triggered() {
	auto fileName = QFileDialog::getOpenFileName(this, tr("Open World"), QString(), tr("World File (*.wld)"));
	ui->viewport->load(fileName);
	m_lastFile = fileName;
}

void MainWindow::on_actionSave_as_triggered() {
	auto fileName = QFileDialog::getSaveFileName(this, tr("Save World"), QString(), tr("World File (*.wld)"));
	ui->viewport->save(fileName);
	m_lastFile = fileName;
}

void MainWindow::on_action_Save_triggered() {
	if (m_lastFile.isEmpty())
		on_actionSave_as_triggered();
	else
		ui->viewport->save(m_lastFile);
}

void MainWindow::on_actionWireframe_toggled(bool checked) {
	if (checked) {
		ui->viewport->renderMode(GL_LINES);
	} else {
		ui->viewport->renderMode(GL_TRIANGLES);
	}
}
