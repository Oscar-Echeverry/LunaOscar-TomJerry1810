#ifndef RECURSOSGRAFICOS_H
#define RECURSOSGRAFICOS_H
#include <QHash>
#include <QPixmap>
#include <QVector>
#include <QStringList>

class RecursosGraficos
{
public:
    RecursosGraficos();
    const QPixmap &fotograma(const QString &nombre, double tiempo, double cuadrosPorSegundo = 10) const;
    const QPixmap &obtenerFondo() const;
    const QPixmap &obtenerFondoTienda() const;
    const QStringList &obtenerErrores() const;
private:
    QPixmap cargarImagen(const QString &ruta, const QRect &region = QRect());
    void cargarAnimacion(const QString &nombre, int carpeta, const QString &prefijo, int cantidad);
    void cargarRecursos();
    void prepararFondo();
    QHash<QString, QVector<QPixmap>> animaciones;
    QVector<QPixmap> fondos;
    QPixmap fondoContinuo;
    QPixmap fondoTienda;
    QStringList recursosFaltantes;
    double anchoMundo = 0;
    double puertaX = 0;
    static constexpr int anchoVista = 1120;
    static constexpr int altoVista = 630;
};
#endif
