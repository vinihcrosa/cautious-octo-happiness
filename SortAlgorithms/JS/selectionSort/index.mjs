export const selectionSort = (arr) => {
  let iteractions = 0;
  let comparisons = 0;
  let swaps = 0;
  for (let i = 0; i < arr.length; i++) {
    iteractions++;
    let minIndex = i;
    for (let j = i + 1; j < arr.length; j++) {
      iteractions++;
      if (arr[j] < arr[minIndex]) {
        minIndex = j;
        comparisons++;
      }
    }
    if (minIndex !== i) {
      comparisons++;
      [arr[i], arr[minIndex]] = [arr[minIndex], arr[i]];
      swaps++;
    }
  }
  return {
    arr,
    swaps,
    comparisons,
    iteractions
  };
} 