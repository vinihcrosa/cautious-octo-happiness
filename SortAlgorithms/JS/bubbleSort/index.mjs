export const bubbleSort = (array) => {
  const arr = [...array];
  let iteractions = 0;
  let comparisons = 0;
  let swaps = 0;
  for (let i = 0; i < arr.length; i++) {
    let swapped = false;
    iteractions++;
    for (let j = 0; j < arr.length - i - 1; j++) {
      iteractions++;
      if (arr[j] > arr[j + 1]) {
        comparisons++;
        [arr[j], arr[j + 1]] = [arr[j + 1], arr[j]];
        swaps++;
        swapped = true;
      }
    }
    if (!swapped) {
      comparisons++;
      break;
    }
  }
  return {
    arr,
    swaps,
    comparisons,
    iteractions
  };
}