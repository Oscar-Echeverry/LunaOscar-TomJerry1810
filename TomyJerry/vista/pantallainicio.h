#ifndef PANTALLAINICIO_H
#define PANTALLAINICIO_H

#include <QFrame>

namespace Ui { class PantallaInicio; }

class PantallaInicio : public QFrame
{
    Q_OBJECT
public:
    explicit PantallaInicio(QWidget *parent = nullptr);
    ~PantallaInicio() override;

signals:
    void jugar();

private:
    Ui::PantallaInicio *ui;
};

#endif