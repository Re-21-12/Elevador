interface Elevador {
  nivel: number;
  niveles: boolean[];
  nombre: string;
}

function posicionElevador(niveles: boolean[]): number {
  const nivel = niveles.findIndex((nivel) => nivel === true);
  return nivel;
}

const elevador1: Elevador = {
  nivel: posicionElevador([false, true, false, false, false]), // Nivel 1
  niveles: [false, true, false, false, false],
  nombre: "Elevador 1",
};

const elevador2: Elevador = {
  nivel: posicionElevador([false, false, true, false, false]), // Nivel 2
  niveles: [false, false, true, false, false],
  nombre: "Elevador 2",
};

const elevadores = [elevador1, elevador2];

function buscarElevador(
  elevadores: Elevador[],
  nivelDestino: number,
): Elevador {
  let mejorElevador = elevadores[0];
  let menorDistancia = Math.abs(elevadores[0].nivel - nivelDestino);

  for (let i = 1; i < elevadores.length; i++) {
    const distancia = Math.abs(elevadores[i].nivel - nivelDestino);
    if (distancia < menorDistancia) {
      menorDistancia = distancia;
      mejorElevador = elevadores[i];
    }
  }

  return mejorElevador;
}

// Función para simular el movimiento
function simularMovimiento(elevador: Elevador, nivelDestino: number): void {
  console.log(
    `\n🚀 ${elevador.nombre} se mueve del nivel ${elevador.nivel} al nivel ${nivelDestino}`,
  );

  const direccion = nivelDestino > elevador.nivel ? "⬆️" : "⬇️";
  const pasos = Math.abs(nivelDestino - elevador.nivel);

  console.log(`${direccion} Distancia: ${pasos} niveles`);

  // Actualizar posición del elevador
  elevador.nivel = nivelDestino;
  elevador.niveles = elevador.niveles.map((_, index) => index === nivelDestino);

  console.log(`✅ ${elevador.nombre} llegó al nivel ${nivelDestino}`);
}

// Pruebas del algoritmo
console.log("🏢 Sistema de Elevadores - Simulación");
console.log("=====================================");

console.log("\n📊 Estado inicial:");
elevadores.forEach((elevador) => {
  console.log(`${elevador.nombre}: Nivel ${elevador.nivel}`);
});

// Buscar elevador para nivel 3
const nivelSolicitado = 3;
const elevadorSeleccionado = buscarElevador(elevadores, nivelSolicitado);

console.log(`\n🎯 Solicitud para nivel ${nivelSolicitado}`);
console.log(`🏆 Elevador seleccionado: ${elevadorSeleccionado.nombre}`);

// Simular el movimiento
simularMovimiento(elevadorSeleccionado, nivelSolicitado);

console.log("\n📊 Estado final:");
elevadores.forEach((elevador) => {
  console.log(`${elevador.nombre}: Nivel ${elevador.nivel}`);
});
