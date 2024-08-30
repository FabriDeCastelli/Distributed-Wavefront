import pandas as pd


def read_MPI(file_path='../outputs/MPI/collective.csv'):
    df = pd.read_csv(file_path, delimiter=';')
    df['nodes'] = df['nodes'].astype(int)
    df['n'] = df['n'].astype(int)
    df['p'] = df['p'].astype(int)
    df['time'] = df['time'].apply(lambda x: list(map(float, x.split(','))))
    return df


def read_sequential(file_path='../outputs/sequential/sequential.csv'):
    df = pd.read_csv(file_path, delimiter=';')
    df['n'] = df['n'].astype(int)
    return df


def read_fastflow(file_path='../outputs/fastflow/fastflow.csv'):
    df = pd.read_csv(file_path, delimiter=';')
    df['n'] = df['n'].astype(int)
    df['w'] = df['w'].astype(int)
    return df
