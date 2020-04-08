import numpy as np
from numpy.fft import fft, ifft, fftshift, fftfreq
from scipy.signal import max_len_seq
import cv2


def gold_seq(nbits, state1=None, state2=None, taps1=None, taps2=None):
    """
    >>> gold_seq(4, state1=[1, 1, 1, 1], state2=[1, 1, 1, 1], taps1=[3], taps2=[1])
    array([0., 0., 0., 0., 0., 1., 0., 0., 1., 0., 1., 0., 0., 1., 0.],
          dtype=float32)
    """
    mls1, _ = max_len_seq(nbits, state=state1, taps=taps1)
    mls2, _ = max_len_seq(nbits, state=state2, taps=taps2)
    seq = (mls1 + mls2) % 2

    return seq.astype(np.float32)

def correlate(seq1, seq2):
    seq1 = normalize(seq1)
    seq2 = normalize(seq2)
    return (ifft(fft(seq1) * np.conj(fft(seq2))).real) + (ifft(fft(1-seq1) * np.conj(fft(1-seq2))).real)

def normalize(seq):
    """
    >>> normalize([2, 0, 4])
    array([0.5, 0. , 1. ], dtype=float32)
    """
    return ((seq - np.min(seq)) / (np.max(seq) - np.min(seq))).astype(np.float32)

def generate_test_video_seq():
    #cap = cv2.VideoCapture('hogehoge.avi')
    #cap.set(3, 640)
    #cap.set(4, 480)

    #i = 0
    #seq = np.zeros((60, 3, 480, 640), dtype=np.uint16)
    
    #while(cap.isOpened()):
    #    _, frame = cap.read()
    #    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    #    seq[i] = frame.transpose(1, 0, 2).transpose(2, 1, 0)
    #    i += 1
    #    if i == 60:
    #        break
    
    #cap.release()
    #cv2.destroyAllWindows()

    seq = (np.random.rand(60, 3, 480, 640) * 255).astype(np.uint16)

    blinky_w = np.ones(60)
    blinky_r = np.tile(gold_seq(5, state1=[1, 1, 1, 0, 0], state2=[1, 1, 1, 0, 0], taps1=[3], taps2=[2, 3, 4]), 2)[:60]
    blinky_g = np.tile(gold_seq(5, state1=[1, 1, 0, 0, 0], state2=[1, 0, 1, 0, 0], taps1=[3], taps2=[2, 3, 4]), 2)[:60]
    blinky_b = np.tile(gold_seq(5, state1=[1, 0, 0, 0, 0], state2=[1, 0, 0, 0, 0], taps1=[3], taps2=[2, 3, 4]), 2)[:60]

    blinky_seq = np.zeros((60, 3, 480, 640), dtype=np.int8)
    pos_w = (480 // 3, 640 // 3)
    pos_r = (480 // 3, 640 // 3 * 2)
    pos_g = (480 // 3 * 2, 640 // 3 * 2)
    pos_b = (480 // 3 * 2, 640 // 3)

    blinky_seq[:, 0, pos_w[0], pos_w[1]] = (blinky_w * 255).astype(np.uint16)
    blinky_seq[:, 1, pos_w[0], pos_w[1]] = (blinky_w * 255).astype(np.uint16) 
    blinky_seq[:, 2, pos_w[0], pos_w[1]] = (blinky_w * 255).astype(np.uint16)

    blinky_seq[:, 0, pos_r[0], pos_r[1]] = (blinky_r * 255).astype(np.uint16) 
        
    blinky_seq[:, 1, pos_g[0], pos_g[1]] = (blinky_g * 255).astype(np.uint16) 
        
    blinky_seq[:, 2, pos_b[0], pos_b[1]] = (blinky_b * 255).astype(np.uint16) 

    return np.clip(seq + blinky_seq, 0, 255).astype(np.uint8)
        
    
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

    seq = generate_test_video_seq()
    print(seq.shape)
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = cv2.VideoWriter('output.avi', fourcc, 30.0, (640, 480)) 
    for i in range(60):
        frame = cv2.cvtColor(seq[i].transpose(2, 1, 0).transpose(1, 0, 2), cv2.COLOR_RGB2BGR)
        out.write(frame)
        cv2.imshow('frame', frame)
        if cv2.waitKey(30) & 0xFF == ord('q'):
            break
    out.release()
    cv2.destroyAllWindows()
