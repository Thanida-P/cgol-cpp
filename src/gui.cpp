#include "gui.hpp"
#include "cgol.hpp"

#include <QtWidgets>
#include <QButtonGroup>
#include <memory>

SimWidget::SimWidget(QWidget *parent, Grid g): QWidget(parent), sim(g) {
    create();
}

void SimWidget::create() {
    setMouseTracking(true);

    timer = std::make_unique<QTimer>(this);
    connect(timer.get(), SIGNAL(timeout()), this, SLOT(update_sim()));

    timer->start(sim.get_delay());
}

void SimWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const qreal cellSize = std::min((qreal)width() / sim.get_width(), (qreal)height() / sim.get_height());
    
    QRectF bg(0, 0,  cellSize*sim.get_width()-1, cellSize*sim.get_height()-1);
    painter.fillRect(bg, QBrush("#FFFFFF"));

    for (size_t y = 0; y < sim.get_height(); ++y) {
        for (size_t x = 0; x < sim.get_width(); ++x) {
            if (sim.get_cell(x, y)) {
                painter.fillRect(x * cellSize, y * cellSize, cellSize, cellSize, Qt::black);
            } else {
                painter.fillRect(x * cellSize, y * cellSize, cellSize, cellSize, Qt::white);
            }
        }
    }
    painter.setPen(QPen(Qt::lightGray, 0.5));
    for (qreal x = 0; x < sim.get_width(); x++)
        painter.drawLine(QPointF(x*cellSize, 0), QPointF(x*cellSize, sim.get_height()*cellSize));
    for (qreal y = 0; y < sim.get_height(); y++)
        painter.drawLine(QPointF(0, y*cellSize), QPointF(sim.get_width()*cellSize, y*cellSize));

}

void SimWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (timer->isActive()) {
            pause();
        }
        QPoint pos = event->pos();
        
        const qreal cellSize = std::min((qreal)width() / sim.get_width(), (qreal)height() / sim.get_height());

        size_t x = pos.x() / cellSize;
        size_t y = pos.y() / cellSize;

        if (x < sim.get_width() && y < sim.get_height()) {
            sim.set_cell(x, y, tool);
            drawing = true;
            update();
        }
    }
}

void SimWidget::mouseMoveEvent(QMouseEvent* event) {
    if (drawing) {
        QPoint pos = event->pos();
        
        const qreal cellSize = std::min((qreal)width() / sim.get_width(), (qreal)height() / sim.get_height());

        size_t x = pos.x() / cellSize;
        size_t y = pos.y() / cellSize;

        if (x < sim.get_width() && y < sim.get_height()) {
            sim.set_cell(x, y, tool);
            update();
        }
    }
}

void SimWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        drawing = false;
    }
}

void SimWidget::set_pencil() {
    tool = 1;
}

void SimWidget::set_eraser() {
    tool = 0;
}

void SimWidget::reset() {
    sim.reset();
    update();
}

void SimWidget::prev() {
    if (sim.get_tick() > 0) {
        sim.prev();
    }
    update();
}

void SimWidget::next() {
    sim.next();
    update();
}

void SimWidget::pause() {
    timer->stop();
}

void SimWidget::play() {
    timer->start(sim.get_delay());
}

void SimWidget::update_sim() {
    next();
}

GridConfig::GridConfig(QWidget *parent) : QWizard(parent) {
    setPage(0, new QWizardPage);
    setWindowTitle(tr("Grid Configuration"));
    setWizardStyle(QWizard::ModernStyle);
    setButtonText(QWizard::FinishButton, tr("Apply"));
    setOption(QWizard::NoBackButtonOnStartPage);
    adjustWindowSize();
    
    QLabel *widthLabel = new QLabel("Width:");
    QLabel *heightLabel = new QLabel("Height:");
    widthSpinBox = new QSpinBox;
    heightSpinBox = new QSpinBox;
    widthSpinBox->setRange(1,200);
    widthSpinBox->setValue(20);
    heightSpinBox->setRange(1,200);
    heightSpinBox->setValue(20);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(widthLabel, 0, 0);
    layout->addWidget(widthSpinBox, 0, 1);
    layout->addWidget(heightLabel, 1, 0);
    layout->addWidget(heightSpinBox, 1, 1);
    layout->setColumnStretch(1, 1);
    layout->setRowStretch(2, 1);
    setLayout(layout);
}

GridConfig::~GridConfig() {
    delete page(0);
}

int GridConfig::get_width() const {
    return widthSpinBox->value();
}
int GridConfig::get_height() const {
    return heightSpinBox->value();
}

void GridConfig::adjustWindowSize() {
    #ifdef Q_OS_WIN
        setFixedSize(300, 100); // Adjust the window size for Windows
    #elif defined(Q_OS_MACOS)
        setFixedSize(300, 120); // Adjust the window size for macOS
    #elif defined(Q_OS_LINUX)
        setFixedSize(300, 120); // Adjust the window size for Linux
    #else
        setFixedSize(300, 120); // Default window size
    #endif
}

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {

    simWidget = new SimWidget();

    setCentralWidget(simWidget);

    setWindowTitle(tr("Conway's Game of Life"));

    createActions();
    createMenus();

    tool_bar = new QToolBar(this);
    tool_bar->setMovable(false);

    #ifdef Q_OS_LINUX
        QPushButton *reset_button = new QPushButton(QIcon("assets/reset2.png"),"");
        QPushButton *prev_button = new QPushButton(QIcon("assets/prev2.png"),"");
        QPushButton *next_button = new QPushButton(QIcon("assets/next2.png"),"");
        QPushButton *pause_button = new QPushButton(QIcon("assets/pause2.png"),"");
        QPushButton *play_button = new QPushButton(QIcon("assets/play2.png"),"");
        QPushButton *pencil_button = new QPushButton(QIcon("assets/pencil2.png"),"");
        QPushButton *eraser_button = new QPushButton(QIcon("assets/eraser2.png"),"");
    #else
        QPushButton *reset_button = new QPushButton(QIcon("assets/reset.png"),"");
        QPushButton *prev_button = new QPushButton(QIcon("assets/prev.png"),"");
        QPushButton *next_button = new QPushButton(QIcon("assets/next.png"),"");
        QPushButton *pause_button = new QPushButton(QIcon("assets/pause.png"),"");
        QPushButton *play_button = new QPushButton(QIcon("assets/play.png"),"");
        QPushButton *pencil_button = new QPushButton(QIcon("assets/pencil.png"),"");
        QPushButton *eraser_button = new QPushButton(QIcon("assets/eraser.png"),"");
    #endif

    // std::cout << QDir::currentPath().toStdString() << std::endl;

    pencil_button->setCheckable(true);
    eraser_button->setCheckable(true);

    reset_button->setIconSize(QSize(30,30));
    prev_button->setIconSize(QSize(30,30));
    next_button->setIconSize(QSize(30,30));
    pause_button->setIconSize(QSize(30,30));
    play_button->setIconSize(QSize(30,30));
    pencil_button->setIconSize(QSize(30,30));
    eraser_button->setIconSize(QSize(30,30));

    speed_slider = new QSlider(Qt::Horizontal, this);
    speed_slider->setRange(0, 30);
    speed_slider->setValue(10);
    speed_slider->setFixedSize(100, 30);
    QWidget *spacer = new QWidget(this);
    spacer->setFixedSize(20, 30);
    QWidget *spacer2 = new QWidget(this);
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    connect(reset_button, &QPushButton::pressed, this, &MainWindow::reset);
    connect(prev_button, &QPushButton::pressed, this, &MainWindow::prev);
    connect(next_button, &QPushButton::pressed, this, &MainWindow::next);
    connect(pause_button, &QPushButton::pressed, this, &MainWindow::pause);
    connect(play_button, &QPushButton::pressed, this, &MainWindow::play);
    connect(speed_slider, &QSlider::valueChanged, this, &MainWindow::update_speed);
    connect(pencil_button, &QPushButton::pressed, this, &MainWindow::set_pencil);
    connect(eraser_button, &QPushButton::pressed, this, &MainWindow::set_eraser);

    tool_bar->addWidget(reset_button);
    tool_bar->addWidget(prev_button);
    tool_bar->addWidget(next_button);

    tool_bar->addWidget(pause_button);
    tool_bar->addWidget(play_button);

    tool_bar->addWidget(spacer);
    tool_bar->addWidget(speed_slider);

    tool_bar->addWidget(spacer2);

    QButtonGroup *tool_group = new QButtonGroup(this);
    tool_group->setExclusive(true);

    tool_group->addButton(pencil_button);
    tool_group->addButton(eraser_button);

    pencil_button->setChecked(true);

    tool_bar->addWidget(pencil_button);
    tool_bar->addWidget(eraser_button);

    addToolBar(Qt::BottomToolBarArea, tool_bar);

    height_offset = tool_bar->height() + menu_bar->height() + 7;

    setMinimumSize(500, 500+height_offset);

    show();

    createNew();
}

MainWindow::~MainWindow() {
    delete simWidget;
}

void MainWindow::createActions() {
    newAction = new QAction(tr("&New"), this);
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::createNew);
    
    randomAction = new QAction(tr("&Random"), this);
    randomAction->setShortcut(Qt::CTRL | Qt::Key_R);
    connect(randomAction, &QAction::triggered, this, &MainWindow::createNewRandom);
    
    loadAction = new QAction(tr("&Load"), this);
    loadAction->setShortcut(QKeySequence::Open);
    connect(loadAction, &QAction::triggered, this, &MainWindow::loadPattern);
    
    saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::savePattern);
}

void MainWindow::createMenus() {
    menu_bar = menuBar()->addMenu(tr("&File"));
    menu_bar->addAction(newAction);
    menu_bar->addAction(randomAction);
    menu_bar->addAction(loadAction);
    menu_bar->addAction(saveAction);
}

void MainWindow::createNew() {

    GridConfig *config = new GridConfig(this);

    if (config->exec() == QDialog::Accepted) {
        int w = config->get_width();
        int h = config->get_height();
        Grid g(w, h);

        simWidget->replace(g);

        qreal aspectRatio = (qreal)w/h;

        int calculatedHeight = width() / aspectRatio;
        int calculatedWidth = height() * aspectRatio;
        
        // Adjust the width or height to match the aspect ratio
        int new_width = width();
        int new_height = height();
        if (calculatedHeight > height())
            new_width = calculatedWidth;
        else
            new_height = calculatedHeight;

        // Set the new size for the main window
        resize(new_width, new_height+height_offset);

        setMinimumSize(500*aspectRatio, 500+height_offset);
        update();
    }
}

void MainWindow::createNewRandom() {

    Grid g(simWidget->sim.get_width(), simWidget->sim.get_height());

    g.random();

    simWidget->replace(g);

    update();
}

void MainWindow::loadPattern() {
    pause();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Pattern"), QString(),
                                                    tr("Supported Formats (*.rle *.txt *.text *.lif *.life);;"
                                                        "Run-Length Encoded (*.rle);;"
                                                        "Text Files (*.txt *.text);;"
                                                        "Life 1.06 Files (*.lif *.life)"));
    if (!fileName.isEmpty()) {            
        Grid g(simWidget->sim.get_width(), simWidget->sim.get_height());
        Grid g2(fileHandler->read(fileName.toStdString()));
        
        g.place_center(g2);
        
        simWidget->replace(g);
        update();
    }
}

void MainWindow::savePattern() {
    pause();
    QFileDialog dialog;
    dialog.setDefaultSuffix(".rle");
    QString fileName = dialog.getSaveFileName(this, tr("Save Pattern"), QString(),
                                                    tr("Run-Length Encoded (*.rle);;"
                                                        "Text Files (*.txt *.text);;"
                                                        "Life 1.06 Files (*.lif *.life)"));
    try {
        if (!fileName.isEmpty()) {
            const Grid pattern = simWidget->sim.cur().get_minimal();
            fileHandler->save(pattern, fileName.toStdString());
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl; 
    }
}

#include "moc_gui.cpp"