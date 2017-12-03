#include "MainWindow.h"

#include "QApplication"
#include "QtGui"

MainWindow::MainWindow()
{
	m_mesh = new Mesh();
	m_fbxmesh = new FBXMesh();
	m_vmf = new VMFExport();

	QWidget *central_widget = new QWidget();

	// main components
	// ---------------------------------------------------------------------------
	m_opengl = new GLWidget(this, m_mesh);

	m_anim_timeline = new AnimTimeline(this);
	m_anim_timeline->setMaximumHeight(150);


	// side widget
	// ---------------------------------------------------------------------------
//	m_animlist = new AnimList(this);
//	m_animlist->setMinimumSize(400, 600);

	QWidget *side_widget = new QWidget();
	QBoxLayout *side_layout = new QBoxLayout(QBoxLayout::TopToBottom);
	side_layout->setSpacing(3);
	side_layout->setMargin(0);
//	side_layout->addWidget(m_animlist);

	side_widget->setLayout(side_layout);


	// main layout
	// ---------------------------------------------------------------------------
	QGridLayout *layout = new QGridLayout();

	layout->setSpacing(3);
	layout->setMargin(3);

	layout->addWidget(m_opengl, 0, 0);
	layout->addWidget(m_anim_timeline, 1, 0);
	layout->addWidget(side_widget, 0, 1, 2, 1);

	central_widget->setLayout(layout);
	setCentralWidget(central_widget);


	// connect signals and slots
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), m_opengl, SLOT(animate()));
	timer->start(50);

	connect(m_anim_timeline, SIGNAL(sliderChanged(int)), m_opengl, SLOT(setAnimTime(int)));

	// create various stuff
	createActions();
	createMenus();
	createToolbars();
	createStatusbar();

	m_loaded = false;
}

MainWindow::~MainWindow(void)
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{

}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		default:
			QMainWindow::keyReleaseEvent(event);
	}
}

void MainWindow::importMesh()
{
	QString filename = QFileDialog::getOpenFileName(this,
													tr("Open FBX File"),
													"",
													tr("FBX File (*.fbx);;All Files (*.*)"));

	if (filename != NULL)
	{
		m_fbxmesh->load(filename.toStdString(), m_mesh);

		int start = m_mesh->getStartTime();
		int end = m_mesh->getEndTime();

		m_anim_timeline->setRange(start, end);
		m_anim_timeline->resetKeyframes();

		Mesh::SubMesh submesh = m_mesh->getSubMesh(0);

		int num_frames = submesh.pos_keys.size();
		for (int i=0; i < num_frames; i++)
		{
			int time = submesh.pos_keys[i].time;
			m_anim_timeline->insertKeyframe(time);
		}

		m_opengl->bake();
	}
}

bool MainWindow::exportMesh()
{
	QString filename = QFileDialog::getSaveFileName(this,
													tr("Save VMF File"),
													"",
													tr("VMF File (*.vmf);;All Files (*.*)"));

	if (filename != NULL)
	{
		m_vmf->write(filename.toStdString(), m_mesh);
	}

	return true;
}

void MainWindow::exitProgram()
{

}

void MainWindow::createActions()
{
	// file menu
	m_importAction = new QAction(QIcon(":/images/open.png"), tr("&Import"), this);
	m_importAction->setShortcuts(QKeySequence::Open);
	m_importAction->setStatusTip(tr("Import a mesh file"));
	connect(m_importAction, SIGNAL(triggered()), this, SLOT(importMesh()));

	m_exportAction = new QAction(QIcon(":/images/save.png"), tr("&Export"), this);
	m_exportAction->setShortcuts(QKeySequence::Save);
	m_exportAction->setStatusTip(tr("Export a mesh file"));
	connect(m_exportAction, SIGNAL(triggered()), this, SLOT(exportMesh()));

	m_exitAction = new QAction(QIcon(":/images/exit.png"), tr("&Exit"), this);
	m_exitAction->setShortcuts(QKeySequence::Quit);
	m_exitAction->setStatusTip(tr("Exit the application"));
	connect(m_exitAction, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
	m_fileMenu = menuBar()->addMenu(tr("&File"));
	m_fileMenu->addAction(m_importAction);
	m_fileMenu->addAction(m_exportAction);
	m_fileMenu->addSeparator();
	m_fileMenu->addAction(m_exitAction);
}

void MainWindow::createToolbars()
{

}

void MainWindow::createStatusbar()
{

}

void MainWindow::reset()
{
	//m_fbxmesh->load("bonusgate.fbx", m_mesh);
	//m_fbxmesh->load("test3.fbx", m_mesh);
	//m_fbxmesh->load("bonusgate.fbx", m_mesh);
}
