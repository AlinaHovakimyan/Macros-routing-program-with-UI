#include "dyplomaWork.h"
#include <QVBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <iostream>
#include <fstream>
#include <map>

#include "Tokenizer.h"
#include "Parser.h"
#include "Exception.h"
#include "Route.h"
#include "Drawer.h"
#include "MST.h"

chipView::chipView(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	createActions();
	createMenus();
	
	setWindowTitle(tr("Chip View"));
}

#include "editWindow.h"

void chipView::editSettings()
{
	Window* editWindow = new Window();
	editWindow->show();
}

void chipView::showHelp()
{
	QLabel* helpText = new QLabel();
	QFont font = helpText->font();
	font.setPointSize(12);
	helpText->setFont(font);
	helpText->setMinimumSize(500, 100);
	helpText->setText("Hi,\nProgram can be used for solving global routing problems.\nprogram gets as input file, where is described design.");
	helpText->show();
}

extern QWidget centralWidget;

void chipView::readDesign()
{
	QString design_filename = QFileDialog::getOpenFileName(
		this,
		"Open Document",
		QDir::currentPath(),
		"All files (*.*) ;; Document files (*.doc *.rtf)");
	std::ifstream inputStream(design_filename.toStdString(), std::ios::in);
	cultivateInput::Tokenizer* tokenizer = new cultivateInput::Tokenizer();
	tokenizer->tokenize(inputStream);
	cultivateInput::Parser* parser = new cultivateInput::Parser(tokenizer->getTokenPool());
	parser->parse();
	Router* router = new Router();
	sortNets(libs::nets);
	router->route(libs::nets);
	std::vector<libs::Net>::iterator i = libs::created_nets.begin();
	libs::Net net;
	for (; i != libs::created_nets.end(); ++i) {
		m_renderer->drawNet(*i);
	}

	// cisht texn esa nael inchi chi ashxatum
	//m_renderer->drawLines(router->getLines());
	//m_renderer->drawLines(m_lines);
	this->statusBar()->showMessage("Design Readed");
	// karelia loger generacnel lcnel .log file meg.
}

void chipView::createActions()
{
	m_readDesignAct = new QAction(tr("&Read Design"), this);
	m_readDesignAct->setShortcuts(QKeySequence::New);
	m_readDesignAct->setStatusTip(tr("Read design from file"));
	connect(m_readDesignAct, SIGNAL(triggered()), this, SLOT(readDesign()));

	m_editSettingsAct = new QAction(tr("&Edit Settings"), this);
	m_editSettingsAct->setShortcuts(QKeySequence::New);
	m_editSettingsAct->setStatusTip(tr("Change routing constraints values"));
	connect(m_editSettingsAct, SIGNAL(triggered()), this, SLOT(editSettings()));
	
	m_aboutAct = new QAction(tr("&About program"));
	m_aboutAct->setShortcuts(QKeySequence::New);
	m_aboutAct->setStatusTip(tr("Show construction of using tool"));
	connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(showHelp()));
}

void chipView::createMenus()
{
	QMenuBar* menuBarPtr = menuBar();
	m_fileMenu = menuBar()->addMenu(QString("File"));
	m_fileMenu->addAction(m_readDesignAct);
	
	m_editMenu = menuBarPtr->addMenu(tr("&Edit"));
	m_editMenu->addAction(m_editSettingsAct);

	m_helpMenu = menuBarPtr->addMenu(tr("&Help"));
	m_helpMenu->addAction(m_aboutAct);
}

#include <queue>
struct net_inf{
	net_inf() {};
	net_inf(int a, libs::Net n) : area(a), net(n) {};
	int area;
	libs::Net net;
	friend bool operator<(const net_inf& first, const net_inf& second) { return first.area > second.area; }
};

void chipView::sortNets(std::vector<libs::Net>& nets)
{
	std::vector<libs::Net> tempNets = nets;
	std::vector<libs::Net>::iterator i = tempNets.begin();
	std::vector<libs::db_pin>::iterator j;
	std::priority_queue<net_inf> nets_inf;
	std::vector<int> areas;
	int x_min = 0;
	int x_max = 0;
	int y_min = 0;
	int y_max = 0;
	int area = 0;
	for (; i != tempNets.end(); ++i) {
		x_min = x_max = i->pins.begin()->x;
		y_min = y_max = i->pins.begin()->y;
		for (j = i->pins.begin() + 1; j != i->pins.end(); ++j) {
			if (j->x < x_min) {
				x_min = j->x;
			}
			else if (j->x > x_max) {
				x_max = j->x;
			}
			if (j->y < y_min) {
				y_min = j->y;
			}
			else if (j->y > y_max) {
				y_max = j->y;
			}
		}
		area = (x_max - x_min) * (y_max - y_min);
		nets_inf.push(net_inf(area, *i));
	}
	nets.clear();
	net_inf tempInf;
	while (!nets_inf.empty()) {
		tempInf = nets_inf.top();
		nets_inf.pop();
		nets.push_back(tempInf.net);
	}
}