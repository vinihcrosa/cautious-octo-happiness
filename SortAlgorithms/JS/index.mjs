import { Worker } from "worker_threads"
import pLimit from 'p-limit'
import fs from 'node:fs'


const limit = pLimit(10)

const exitFile = fs.createWriteStream('exit.csv')
exitFile.write('algorithm,arrLength,swaps,comparisons,iteractions,time\n')

function testMultiThread(arr, worker) {
  const p = new Promise((resolve, reject) => {
    worker.once('message', (message) => {
      console.log(message.algorithm, message.arrLength)
      exitFile.write(`${message.algorithm},${message.arrLength},${message.swaps},${message.comparisons},${message.iteractions},${message.time}\n`)
      resolve(message)
    })
    worker.once('error', (error) => {
      reject(error)
    })
  })

  worker.postMessage(arr)
  return p
}

const paths = [
  './bubbleSort/bubbleSortThread.mjs',
  './insertionSort/insertionSortThread.mjs',
  './selectionSort/selectionSortThread.mjs'
]

const createArray = (len) => Array.from({ length: len }, () => Math.floor(Math.random() * len))

const promises = []

for (const n of [1000, 10000, 100000, 1000000, 10000000]) {
  const arr = createArray(n)
  for (const path of paths) {
    const worker = new Worker(path)
    promises.push(limit(() => testMultiThread(arr, worker)))
  }
}


Promise.allSettled(promises).then((results) => {
  console.log('All threads finished')
  console.log(results)
}).catch((error) => {
  console.error(error)
})