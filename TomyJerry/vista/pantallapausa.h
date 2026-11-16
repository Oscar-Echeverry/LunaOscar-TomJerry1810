#ifndef PANTALLAPAUSA_H
#define PANTALLAPAUSA_H
#include <QFrame>

namespace Ui { class PantallaPausa; }

class PantallaPausa : public QFrame
{
    Q_OBJECT
public:
    explicit PantallaPausa(QWidget *parent = nullptr);
    ~PantallaPausa() override;

signals:
    void continuar();
    void reiniciar();
    void nuevaPartida();

private:
    Ui::PantallaPausa *ui;
};
#endif
