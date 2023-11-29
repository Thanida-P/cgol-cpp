#ifndef CGOL_GUI_HPP
#define CGOL_GUI_HPP

#include "cgol.hpp"
#include "parser_utils.hpp"

#include <QtWidgets>
#include <QWizard>
#include <memory>

class SimWidget: public QWidget {
    Q_OBJECT
public:
    friend class MainWindow;

    SimWidget(QWidget *parent = nullptr, Grid g = Grid(20, 20));
    
    void replace(const Grid& g) {
        int temp_delay = sim.get_delay();
        sim = Simulation(g);
        sim.set_delay(temp_delay);
    }

    void create();

    void set_pencil();
    void set_eraser();
    void reset();
    void prev();
    void next();
    void pause();
    void play();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

public slots:
    void update_sim();

private:
    Simulation sim;
    bool drawing = 0;
    bool tool = 1;
    std::unique_ptr<QTimer> timer;
};

class GridConfig: public QWizard {
    Q_OBJECT
public:
    friend class MainWindow;

    GridConfig(QWidget *parent = nullptr);
    ~GridConfig();

    int get_width() const;
    int get_height() const;

private:
    QSpinBox *widthSpinBox;
    QSpinBox *heightSpinBox;

    void adjustWindowSize();
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void set_pencil() {
        simWidget->set_pencil();
    }
    void set_eraser() {
        simWidget->set_eraser();
    }
    void reset() {
        pause();
        simWidget->reset();
    }
    void prev() {
        pause();
        simWidget->prev();
    }
    void next() {
        pause();
        simWidget->next();
    }
    void pause() {
        simWidget->pause();
    }
    void play() {
        simWidget->play();
    }
    void update_speed(int speed) {
        simWidget->sim.set_delay(300 * (31-speed)/30);
        simWidget->timer->setInterval(simWidget->sim.get_delay());
    }

protected:
    void resizeEvent(QResizeEvent *event) override {
        QMainWindow::resizeEvent(event);
        qreal aspectRatio = (qreal)simWidget->sim.get_width()/simWidget->sim.get_height();

        QSize currentSize = event->size();
        int width = currentSize.width();
        int height = currentSize.height();
        int calculatedHeight = width / aspectRatio;
        int calculatedWidth = height * aspectRatio;
        
        // Adjust the width or height to match the aspect ratio
        if (calculatedHeight > height)
            width = calculatedWidth;
        else
            height = calculatedHeight;

        // Set the new size for the main window
        resize(width, height+height_offset);
    }

private:
    void createActions();
    void createMenus();

    void createNew();
    void createNewRandom();

    void loadPattern();
    void savePattern();

    SimWidget* simWidget;
    std::unique_ptr<FileHandler> fileHandler;

    int height_offset;

    QMenu *menu_bar;
    QToolBar *tool_bar;
    QSlider *speed_slider;

    QAction *newAction;
    QAction *randomAction;
    QAction *loadAction;
    QAction *saveAction;
};

#endif /* CGOL_GUI_HPP */