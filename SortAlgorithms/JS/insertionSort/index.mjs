export const insertionSort = (arr) => {
  let iteractions = 0;
  let comparisons = 0;
  let swaps = 0;
  const array = [...arr];
  for (let i = 1; i < array.length; i++) {
    iteractions++;
    const current = array[i];
    let j = i - 1;
    while (j >= 0 && array[j] > current) {
      iteractions++;
      comparisons++;
      array[j + 1] = array[j];
      swaps++;
      j--;
    }
    array[j + 1] = current;
  }
  return {
    arr: array,
    swaps,
    comparisons,
    iteractions
  };
}