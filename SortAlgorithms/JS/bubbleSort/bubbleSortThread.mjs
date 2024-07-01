import {
  parentPort
} from 'node:worker_threads'
import { bubbleSort } from './index.mjs';

parentPort?.once('message', (arr) => {
  const array = [...arr];
  const startTime = performance.now();

  const result = bubbleSort(array);

  const endTime = performance.now();

  parentPort?.postMessage({
    algorithm: 'bubbleSort',
    arrLength: arr.length,
    swaps: result.swaps,
    comparisons: result.comparisons,
    iteractions: result.iteractions,
    time: endTime - startTime
  });
})