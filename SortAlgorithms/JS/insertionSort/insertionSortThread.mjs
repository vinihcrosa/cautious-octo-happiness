import {
  parentPort
} from 'node:worker_threads'
import { insertionSort } from './index.mjs';

parentPort?.once('message', (arr) => {
  const array = [...arr];
  const startTime = performance.now();

  const result = insertionSort(array);

  const endTime = performance.now();

  parentPort?.postMessage({
    algorithm: 'insertionSort',
    arrLength: arr.length,
    swaps: result.swaps,
    comparisons: result.comparisons,
    iteractions: result.iteractions,
    time: endTime - startTime
  });
})