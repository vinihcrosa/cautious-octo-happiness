import express from 'express';
import morngan from 'morgan';
import os from 'os';

const app = express();
const port = 3000;

app.use(morngan('dev'));

app.get('/', (req, res) => {
  const { numero } = req.query;
  console.log("chamando o servidor -> ", os.hostname())
  for (let i = 2; i <= Math.sqrt(numero); i++) {
    if (numero % i === 0) {
      res.send({ primo: false });
      return;
    }
  }

  res.send({ primo: true });
})

app.listen(port, () => {
  console.log(`Servidor rodando em http://${os.hostname()}:${port}`);
})