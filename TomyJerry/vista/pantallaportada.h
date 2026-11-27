#ifndef PANTALLAPORTADA_H
#define PANTALLAPORTADA_H

#include <QWidget>
#include <QPixmap>

class QPushButton;

class PantallaPortada : public QWidget
{
    Q_OBJECT
public:
    explicit PantallaPortada(QWidget *parent = nullptr);
signals:
    void continuar();
protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
private:
    QRectF rectanguloImagen() const;
    QRectF rectanguloJugar() const;
    QPixmap imagen;
    QPushButton *botonJugar;
};
#endif
