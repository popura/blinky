import numpy as np
from numpy.fft import fft, ifft, fftshift, fftfreq
from scipy.signal import max_len_seq
import cv2


def gold_seq(nbits, state1=None, state2=None, taps1=None, taps2=None):
    """
    >>> gold_seq(4, state1=[1, 1, 1, 1], state2=[1, 1, 1, 1], taps1=[3], taps2=[1])
    array([0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0], dtype=int8)
    """
    mls1, _ = max_len_seq(nbits, state=state1, taps=taps1)
    mls2, _ = max_len_seq(nbits, state=state2, taps=taps2)
    seq = (mls1 + mls2) % 2

    return seq

def correlate(seq1, seq2):
    seq1 = normalize(seq1)
    seq2 = normalize(seq2)
    #return np.correlate(seq1, seq2, 'full') + np.correlate(1-seq1, 1-seq2, 'full')
    return (ifft(fft(seq1) * np.conj(fft(seq2))).real) + (ifft(fft(1-seq1) * np.conj(fft(1-seq2))).real)

def normalize(seq):
    return (seq - np.min(seq)) / (np.max(seq) - np.min(seq))
        
    
if __name__ == "__main__":
    seq1 = gold_seq(5, state1=[1, 1, 1, 0, 0], state2=[1, 1, 1, 0, 0], taps1=[3], taps2=[2, 3, 4])
    seq2 = gold_seq(5, state1=[1, 1, 0, 0, 0], state2=[1, 0, 1, 0, 0], taps1=[3], taps2=[2, 3, 4])
    seq3 = gold_seq(5, state1=[1, 0, 0, 0, 0], state2=[1, 0, 0, 0, 0], taps1=[3], taps2=[2, 3, 4])

    print(correlate(seq1, seq1))
    print(correlate(seq1, seq2))
    print(correlate(seq1, seq3))
    print(correlate(seq2, seq2))
    print(correlate(seq2, seq3))
    print(correlate(seq3, seq3))
