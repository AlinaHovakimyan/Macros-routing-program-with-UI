#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_dyplomaWork.h"
#include "Drawer.h"

class chipView : public QMainWindow
{
	Q_OBJECT

public:
	chipView(QWidget *parent = Q_NULLPTR);
	void setRenderer(Renderer* renderer) { m_renderer = renderer; };
private slots:
	void readDesign();
	void editSettings();
	void showHelp();
private:
	void createActions();
	void createMenus();
	void sortNets(std::vector<libs::Net>& nets);
private:
	Ui::dyplomaWorkClass ui;
	QMenu* m_fileMenu;
	QMenu* m_editMenu;
	QMenu* m_helpMenu;
	QAction* m_readDesignAct;
	QAction* m_editSettingsAct;
	QAction* m_aboutAct;

	Renderer* m_renderer;
};
