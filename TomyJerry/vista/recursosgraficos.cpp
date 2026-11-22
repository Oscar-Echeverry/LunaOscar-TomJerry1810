#include "recursosgraficos.h"
#include <QImage>
#include <QPainter>
#include <QLinearGradient>
#include <algorithm>
#include <QDebug>

using namespace std;

RecursosGraficos::RecursosGraficos()
{
    cargarRecursos();
    prepararFondo();
}

QPixmap RecursosGraficos::cargarImagen(const QString &ruta, const QRect &region)
{
    QImage imagen(ruta);

    if (imagen.isNull()) {
        recursosFaltantes.append(ruta);
        qWarning("No se pudo cargar: %s", qPrintable(ruta));
        return QPixmap();
    }

    if (region.isValid())
        imagen = imagen.copy(region);

    imagen = imagen.convertToFormat(QImage::Format_ARGB32);

    int izquierda = imagen.width();
    int arriba = imagen.height();
    int derecha = -1;
    int abajo = -1;

    for (int y = 0; y < imagen.height(); ++y) {
        const QRgb *fila = reinterpret_cast<const QRgb *>(imagen.constScanLine(y));

        for (int x = 0; x < imagen.width(); ++x) {
            if (qAlpha(fila[x]) > 8) {
                izquierda = min(izquierda, x);
                derecha = max(derecha, x);
                arriba = min(arriba, y);
                abajo = max(abajo, y);
            }
        }
    }

    if (derecha < izquierda)
        return QPixmap::fromImage(imagen);

    return QPixmap::fromImage(
        imagen.copy(
            izquierda,
            arriba,
            derecha - izquierda + 1,
            abajo - arriba + 1
            )
        );
}

void RecursosGraficos::cargarAnimacion(const QString &nombre, int carpeta,
                                       const QString &prefijo, int cantidad)
{
    QVector<QPixmap> cuadros;

    for (int i = 1; i <= cantidad; ++i) {
        const QString ruta = QString(":/sprites/Sprites/%1/%2_%3.png")
        .arg(carpeta)
            .arg(prefijo)
            .arg(i, 3, 10, QLatin1Char('0'));

        cuadros.append(cargarImagen(ruta));
    }

    animaciones.insert(nombre, cuadros);
}

void RecursosGraficos::cargarRecursos()
{
    const QStringList nombresFondos = {
        QStringLiteral("Balcones coloniales con montanas andinas-1.png"),
        QStringLiteral("Casitas blancas bajo montanas andinas-2.png"),
        QStringLiteral("Plaza de mercado colonial en Bogota-3.png"),
        QStringLiteral("Entrada colonial al barrio mercantil-5.png"),
        QStringLiteral("Tienda Llorente en la calle colonial-4.png")
    };

    for (const QString &nombre : nombresFondos) {
        const QString ruta = ":/Pantallas/Pantallas/Nivel 1/" + nombre;
        QPixmap fondo(ruta);

        if (fondo.isNull()) {
            recursosFaltantes.append(ruta);
            qWarning() << "IMAGEN NO ENCONTRADA:" << ruta;
        } else {
            qDebug() << "IMAGEN CARGADA:" << ruta;
        }

        fondos.append(fondo);
    }

    cargarAnimacion("jerry_quieto", 6, "quieto", 4);
    cargarAnimacion("jerry_caminar", 6, "caminar", 6);
    cargarAnimacion("jerry_correr", 6, "correr", 6);
    cargarAnimacion("jerry_saltar", 6, "saltar", 5);
    cargarAnimacion("jerry_herido", 6, "herido", 5);
    cargarAnimacion("jerry_caer", 6, "caer", 2);

    const QString caida = ":/sprites/Sprites/6/caer_003.png";

    animaciones["jerry_caer"].append(
        cargarImagen(caida, QRect(0, 0, 220, 100))
        );

    animaciones["jerry_caer"].append(
        cargarImagen(caida, QRect(220, 0, 183, 100))
        );

    cargarAnimacion("tom_quieto", 4, "quieto", 5);
    cargarAnimacion("tom_caminar", 4, "caminar", 6);
    cargarAnimacion("tom_correr", 4, "correr", 6);

    cargarAnimacion("campesino", 2, "campesino", 2);
    cargarAnimacion("dama", 2, "dama", 4);
    cargarAnimacion("nino", 5, "nino_caminar", 4);
    cargarAnimacion("nina", 5, "nina_caminar", 4);
    cargarAnimacion("perro", 5, "perro_caminar", 3);

    cargarAnimacion("moneda", 1, "moneda", 3);

    const QStringList nombres = {
        "queso_001",
        "llave_001",
        "caja_001",
        "barril_001"
    };

    for (const QString &nombre : nombres) {
        animaciones.insert(
            nombre,
            {cargarImagen(":/sprites/Sprites/1/" + nombre + ".png")}
            );
    }
}

void RecursosGraficos::prepararFondo()
{
    const int solapamiento = 140;

    const int anchoPanel = fondos.first().isNull()
                               ? anchoVista
                               : fondos.first().scaledToHeight(altoVista).width();

    anchoMundo = anchoPanel * 5 - solapamiento * 4;

    puertaX = (anchoPanel - solapamiento) * 4 + anchoPanel * 0.79;

    fondoContinuo = QPixmap(int(anchoMundo), altoVista);

    fondoContinuo.fill(QColor(116, 175, 205));

    QPainter panorama(&fondoContinuo);

    for (int i = 0; i < fondos.size(); ++i) {
        QImage panel = fondos[i]
                           .scaledToHeight(altoVista, Qt::SmoothTransformation)
                           .toImage()
                           .convertToFormat(QImage::Format_ARGB32_Premultiplied);

        if (i > 0 && !panel.isNull()) {
            QPainter mezcla(&panel);

            mezcla.setCompositionMode(
                QPainter::CompositionMode_DestinationIn
                );

            QLinearGradient degradado(0, 0, solapamiento, 0);

            degradado.setColorAt(0, Qt::transparent);
            degradado.setColorAt(1, Qt::white);

            mezcla.fillRect(panel.rect(), degradado);
        }

        panorama.drawImage(
            i * (anchoPanel - solapamiento),
            0,
            panel
            );
    }
}

const QPixmap &RecursosGraficos::fotograma(const QString &nombre,
                                           double relojAnimacion,
                                           double cuadrosPorSegundo) const
{
    static const QPixmap vacio;

    const auto grupo = animaciones.constFind(nombre);

    if (grupo == animaciones.constEnd() || grupo.value().isEmpty())
        return vacio;

    const int indice = int(relojAnimacion * cuadrosPorSegundo)
                       % int(grupo.value().size());

    return grupo.value()[indice];
}

const QPixmap &RecursosGraficos::obtenerFondo() const
{
    return fondoContinuo;
}

const QStringList &RecursosGraficos::obtenerErrores() const
{
    return recursosFaltantes;
}