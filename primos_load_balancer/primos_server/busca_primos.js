import axios from 'axios';
import pLimit from 'p-limit';
import fs from 'fs';

const limit = pLimit(1000);

const isPrime = async (number) => {
  const { data } = await axios.get(`http://localhost:3000/?numero=${number}`);
  return data.primo ? number : null;
}

function formatTime(time) {
  const minutes = Math.floor(time / 60000);
  const seconds = ((time % 60000) / 1000).toFixed(0);
  return `${minutes}:${seconds < 10 ? '0' : ''}${seconds}`;
}

async function buscaPrimos() {
  const inicio = Date.now();
  const numeros = [];

  for (let i = 2; i <= 10000000; i++) {
    numeros.push(limit(() => isPrime(i)));
  }

  const primos = await Promise.all(numeros);

  await fs.writeFileSync('primos.txt', primos.filter(Boolean).join(','));

  console.log(`Tempo de execução: ${formatTime(Date.now() - inicio)}ms`);
}

buscaPrimos();