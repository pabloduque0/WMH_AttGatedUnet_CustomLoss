# AttUnet_WeightedDSCLoss

## Output

Example output is provided in the output folder of the repo.

## Demo

We recommend running it using Docker.

docker build -f Dockerfile -t [NAME] .

docker run -dit -v [TEST-ORIG]:/input/orig:ro -v [TEST-PRE]:/input/pre:ro -v /output [NAME]

docker exec [CONTAINER-ID] python3 /wmhseg_example/main.py


## Data

Data is provided in the WMH challenge:

https://wmh.isi.uu.nl/
