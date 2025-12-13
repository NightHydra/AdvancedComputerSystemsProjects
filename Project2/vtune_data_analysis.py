import pandas as pd

VTUNE_PATH = "vtune_results/"

df = pd.read_csv(VTUNE_PATH+"walk_size_1000.csv")

print (list(df["Hardware Event Count:MEM_LOAD_RETIRED.L1_MISS"]))
