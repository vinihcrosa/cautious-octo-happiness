import {
  parentPort
} from 'node:worker_threads'
import { selectionSort } from './index.mjs';

parentPort?.once('message', (arr) => {
  const array = [...arr];
  const startTime = performance.now();

  const result = selectionSort(array);

  const endTime = performance.now();

  parentPort?.postMessage({
    algorithm: 'selectionSort',
    arrLength: arr.length,
    swaps: result.swaps,
    comparisons: result.comparisons,
    iteractions: result.iteractions,
    time: endTime - startTime
  });
})