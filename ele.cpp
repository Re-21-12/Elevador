#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>
#include <chrono>

struct Elevador {
    int nivel;
    bool niveles[5];
    const char* nombre;
};

Elevador elevador1 = {
    1, {false, true, false, false, false}, "Elevador 1"
};

Elevador elevador2 = {
    2, {false, false, true, false, false}, "Elevador 2"
};

Elevador* elevadores[] = { &elevador1, &elevador2 };
const int numElevadores = 2;

void imprimirNiveles(const Elevador* elevador) {
    std::cout << "\n🏗️ Estado de " << elevador->nombre << "\n";
    for (int i = 4; i >= 0; i--) {
        std::cout << "Nivel " << i << ": "
                  << (elevador->nivel == i ? "🟢" : "⚪") << "\n";
    }
}

Elevador* buscarElevador(Elevador* elevadores[], int numElevadores, int nivelDestino) {
    Elevador* mejorElevador = elevadores[0];
    int menorDistancia = std::abs(elevadores[0]->nivel - nivelDestino);

    for (int i = 1; i < numElevadores; i++) {
        int distancia = std::abs(elevadores[i]->nivel - nivelDestino);
        if (distancia < menorDistancia) {
            menorDistancia = distancia;
            mejorElevador = elevadores[i];
        }
    }
    return mejorElevador;
}

void simularMovimiento(Elevador* elevador, int nivelDestino) {
    if (nivelDestino < 0 || nivelDestino > 4) {
        std::cout << "❌ Nivel destino inválido. Debe estar entre 0 y 4.\n";
        return;
    }

    std::cout << "\n🚀 " << elevador->nombre << " se mueve del nivel "
              << elevador->nivel << " al nivel " << nivelDestino << "\n";

    const char* direccion = (nivelDestino > elevador->nivel) ? "⬆️" : "⬇️";
    int pasos = std::abs(nivelDestino - elevador->nivel);

    std::cout << direccion << " Distancia: " << pasos << " niveles\n";

    elevador->nivel = nivelDestino;
    for (int i = 0; i < 5; i++) {
        elevador->niveles[i] = (i == nivelDestino);
    }

    std::cout << "✅ " << elevador->nombre << " llegó al nivel " << nivelDestino << "\n";
    imprimirNiveles(elevador);
}

int main() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << "🏢 Sistema de Elevadores - Simulación\n";
    std::cout << "=====================================\n";

    std::cout << "\n📊 Estado inicial:\n";
    for (int i = 0; i < numElevadores; i++) {
        std::cout << elevadores[i]->nombre << ": Nivel " << elevadores[i]->nivel << "\n";
        imprimirNiveles(elevadores[i]);
    }

    int nivelSolicitado = 3;
    std::cout << "\n🎯 Solicitud para nivel " << nivelSolicitado << "\n";

    Elevador* elevadorSeleccionado = buscarElevador(elevadores, numElevadores, nivelSolicitado);

    std::cout << "🏆 Elevador seleccionado: " << elevadorSeleccionado->nombre << "\n";

    simularMovimiento(elevadorSeleccionado, nivelSolicitado);

    std::cout << "\n📊 Estado final:\n";
    for (int i = 0; i < numElevadores; i++) {
        std::cout << elevadores[i]->nombre << ": Nivel " << elevadores[i]->nivel << "\n";
        imprimirNiveles(elevadores[i]);
    }

    return 0;
}
