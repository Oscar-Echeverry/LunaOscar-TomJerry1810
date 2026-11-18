#ifndef PANTALLARESULTADO_H
#define PANTALLARESULTADO_H
#include <QFrame>

namespace Ui { class PantallaResultado; }

class PantallaResultado : public QFrame
{
    Q_OBJECT
public:
    explicit PantallaResultado(QWidget *parent = nullptr);
    ~PantallaResultado() override;
    void mostrarResultado(bool victoria, int quesos, int puntos);

signals:
    void repetir();
    void nuevaPartida();

private:
    Ui::PantallaResultado *ui;
};
#endif
