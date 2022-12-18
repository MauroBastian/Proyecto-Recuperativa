#include <iostream>
#include <fstream>
#include <string>
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <algorithm>

// Estructura de un equipo

struct Equipo {
  std::string nombre;
  std::string comuna;
  std::string estadio;
  double latitud;
  double longitud;
};

// Utilizando la fórmula de Haversine

double HaversineDistance(double latitud_1, double longitud_1, double latitud_2, double longitud_2) {
  // Conversión de coordenadas a radianes
  const double PI = 3.1416;
  latitud_1 = latitud_1 * PI / 180.0;
  longitud_1 = longitud_1 * PI / 180.0;
  latitud_2 = latitud_2 * PI / 180.0;
  longitud_2 = longitud_2 * PI / 180.0;

  // Diferencias entre las coordenadas
  double dif_Latitud = latitud_2 - latitud_1;
  double dif_Longitud = longitud_2 - longitud_1;

  // Distancia utilizando la fórmula de Haversine
  double a = sin(dif_Latitud / 2) * sin(dif_Latitud / 2) + cos(latitud_1) * cos(latitud_2) * sin(dif_Longitud / 2) * sin(dif_Longitud / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));

  // Distancia en metros
  return c * 6371000;
}

//Función para separar Strings

std::vector<std::string> split(const std::string& s, char separator) {
  std::vector<std::string> fields;
  std::string field;
  for (char c : s) {
    if (c == separator) {
      fields.push_back(field);
      field.clear();
    } else {
      field += c;
    }
  }
  fields.push_back(field);
  return fields;
}


int main(int argc, char *argv[])
{

//ABRIR, LEER, GUARDAR Y CERRAR EL ARCHIVO CON LA INFORMACION DE LOS EQUIPOS

    // Abrir el archivo de entrada .txt
    std::ifstream archivo_entrada("equipos.txt");
    if (!archivo_entrada.is_open()) {
        std::cerr << "NO SE PUEDE ABRIR EL ARCHIVO DE ENTRADA" << std::endl;
        return 1;
    }

    // Leer la primera línea del archivo que es el encabezado
    std::string encabezado;
    std::getline(archivo_entrada, encabezado);

    // Guarda los equipos leídos en un vector
    std::vector<Equipo> equipos;

    // Lee cada línea del archivo y guarda la información de cada equipo en un vector
    std::string linea;

    while (std::getline(archivo_entrada, linea))
    {
        // Separar la línea por el caracter ';'
        std::vector<std::string> fields = split(linea, ';');

        // Verificar que todas las lineas se hayan leido
        if (fields.size() != 5)
        {
            std::cerr << "Error: formato de archivo incorrecto" << std::endl;
            return 1;
        }

        // Crear una estructura con la información del archivo de tipo Equipo
        Equipo e;
        e.nombre = fields[0];
        e.comuna = fields[1];
        e.estadio = fields[2];
        e.latitud = std::stod(fields[3]);
        e.longitud = std::stod(fields[4]);

        // Agregar el equipo al vector
        equipos.push_back(e);
    }

    // Cierra el archivo de entrada
    archivo_entrada.close();

    // Obtiene la cantidad de equipos
    int num_equipos = equipos.size();

    // Crear la matriz de distancias
    std::vector<std::vector<double>> distancias(num_equipos, std::vector<double>(num_equipos));

//-.-.-.-.-..-.-.-.-.-.-..-.-.-.-.-.-..-.-.-.-.-.-..-.-.-.-.-.-..-.-.-.-.-.-..-.-.-.-.-.-..-.-.-.-.-.-..-.-.-.-.-.-..-.

#pragma omp parallel for


//CALCULO DE LA DISTANCIA DE LOS EQUIPOS QUE SE ENFRENTAN CON LA FORMULA DE HAVERSINE

    // Calcular la distancia entre cada de equipos
    for (int i = 0; i < num_equipos; i++)
    {
        for (int j = 0; j < num_equipos; j++)
        {
            // Si i es igual a j, la distancia es 0
            if (i == j)
            {
                //Guarda las distancia en una matriz
                distancias[i][j] = 0.0;
                continue;
            }

            // Distancia entre los equipos i y j con fórmula de Haversine
            double distancia = HaversineDistance(equipos[i].latitud, equipos[i].longitud, equipos[j].latitud, equipos[j].longitud);

            // Distancia guardada de forma matricial
            distancias[i][j] = distancia;
            distancias[j][i] = distancia;
        }
    }
//-.-.-.-.-..-.-.-.-.-.-..-.-.-.-.-.-..-.-.-.-.-.-..-.-.-.-.-.-..-.-.-.-.-.-..-.-.-.-.-.-..-.-.-.-.-.-..-.-.-.-.-.-..-.

// GENERACIÓN DEL ARCHIVO CSV, CON LA INFORMACION DEL FIXTURE

    std::ofstream archivo_salida("fixture.csv");
    if (!archivo_salida.is_open()) {
        std::cerr << "Error: no se puede crear el archivo de salida" << std::endl;
        return 1;
  }

  //Escribir el encabezado del archivo
  archivo_salida << "Fecha de partido;Equipo Local;Equipo Visitante;Distancia recorrida por equipo visitante[mts]" << std::endl;

  // Escribir la información de cada partido en el archivo
  for (int i = 0; i < num_equipos; i++) {
    for (int j = i + 1; j < num_equipos; j++) {
      // Escribir: fecha, equipo local, equipo visitante y distancia entre los estadios en el archivo CSV
      archivo_salida << i + 1 << ";\"" << equipos[i].nombre << "\";\"" << equipos[j].nombre << "\";" << distancias[i][j] << std::endl;
    }
  }
  // Cierre de archivo de salida
  archivo_salida.close();

  return 0;
}
