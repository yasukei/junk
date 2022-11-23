import sys
import random
import statistics
import numpy as np
import numpy.linalg as LA

def randomizeCode(eps, code):
    code = [ False if code[i] == '0' else True for i in range(len(code)) ]

    for i in range(len(code)):
        code[i] = not code[i] if random.uniform(0.0, 1.0) <= eps else code[i]

    code = [ '1' if code[i] else '0' for i in range(len(code)) ]
    return ''.join(code)

def smoothCode(code, radius):
    code = [ False if code[i] == '0' else True for i in range(len(code)) ]

    for i in range(len(code)):
        l = i - radius if i - radius >= 0 else 0
        r = i + radius if i + radius < len(code) else len(code)-1

        one = 0
        zero = 0
        for j in range(l, r+1):
            one += 1 if code[j] else 0
            zero += 1 if not code[j] else 0
        if one > zero:
            code[i] = True
        elif one < zero:
            code[i] = False
        else:
            pass

    code = [ '1' if code[i] else '0' for i in range(len(code)) ]
    return ''.join(code)

def simulateScore(eps, n, codes, decoder, repeat=10):
    scoreTotal = 0
    errorTotal = 0
    for _ in range(repeat):
        error = 0
        for _ in range(100):
            correctIndex =  random.randint(0, len(codes)-1)
            randomizedCode = randomizeCode(eps, codes[correctIndex])
            speculatedIndex = decoder(randomizedCode)
            if(speculatedIndex != correctIndex):
                error += 1
        scoreTotal += int(10**9 * 0.9**error / n)
        errorTotal += error

    return int(scoreTotal / repeat), int(errorTotal / repeat)

class Graph:
    def __init__(self, n=2):
        self.matrix = np.zeros((n, n), dtype=int)

    def __str__(self):
        result = ''
        for i in range(self.matrix.shape[0]):
            row = ['#' if self.matrix[i][j] == 1 else ' ' for j in range(self.matrix.shape[0])]
            result += ''.join(row)
            result += '\n'
        return result

    def encode(self):
        code = [ str(self.matrix[i][j]) if j > i else '' for i in range(self.matrix.shape[0]) for j in range(self.matrix.shape[0]) ]
        return ''.join(code)

    def decode(self, codeString):
        n = 2
        while n*(n-1)/2 != len(codeString):
            n += 1
            assert n <= 100
        self.matrix = np.zeros((n, n), dtype=int)
        index = 0
        for i in range(self.matrix.shape[0]):
            for j in range(self.matrix.shape[0]):
                if j > i:
                    self.matrix[i][j] = codeString[index]
                    self.matrix[j][i] = codeString[index]
                    index += 1
        return self

    def eigen(self):
        W, _ = LA.eig(self.matrix)
        W = np.round(np.sort(W), 6)
        return tuple(map(tuple, [W]))[0]

    def makeAllConnected(self):
        shape = self.matrix.shape
        rowNum = shape[0]
        colNum = shape[1]
        self.matrix = np.ones(shape) - np.eye(rowNum, colNum)

    def multiply(self, times):
        old = self.matrix
        new = np.zeros((old.shape[0] * times, old.shape[0] * times), dtype=int)
        for i in range(old.shape[0]):
            for j in range(old.shape[0]):
                if j > i:
                    for k in range(times):
                        for l in range(times):
                            new[i*times + k][j*times + l] = old[i][j]
                if j < i:
                    for k in range(times):
                        for l in range(times):
                            new[i*times + k][j*times + l] = old[i][j]
        self.matrix = new
        return self

    def smooth(self, radius):
        old = self.matrix
        new = self.matrix
        for i in range(old.shape[0]):
            for j in range(old.shape[0]):
                if i == j:
                    continue
                l = j - radius if j - radius >= 0 else 0
                r = j + radius if j + radius < old.shape[0] else old.shape[0] - 1
                t = i - radius if i - radius >= 0 else 0
                b = i + radius if i + radius < old.shape[0] else old.shape[0] - 1

                one = 0
                zero = 0
                for x in range(t, b+1):
                    for y in range(l, r+1):
                        one += 1 if old[x][y] == 1 else 0
                        zero += 1 if old[x][y] == 0 else 0
                        num += 1
                if one > zero:
                    new[i][j] = 1
                elif one < zero:
                    new[i][j] = 0
                else:
                    new[i][j] = old[i][j]
        self.matrix = new
        return self

    @staticmethod
    def normBetweenEigens(eigen1, eigen2):
        e1 = np.array(eigen1)
        e2 = np.array(eigen2)
        return LA.norm(e1 - e2)

    @staticmethod
    def findNearestEigenIndex(targetEigen, eigens):
        norms = [ Graph.normBetweenEigens(targetEigen, eigen) for eigen in eigens ]
        return norms.index(min(norms))

class CodeGenerator:
    @staticmethod
    def makeMinimumCode(m):
        if m <= len(CodeToEigenN4):
            d = CodeToEigenN4
            n = 4
        elif m <= len(CodeToEigenN5):
            d = CodeToEigenN5
            n = 5
        elif m <= len(CodeToEigenN6):
            d = CodeToEigenN6
            n = 6
        else:
            assert m <= len(gDictN6)

        codes = list(d.keys())[:m]
        eigens = list(d.values())[:m]
        return n, codes, lambda code: Graph.findNearestEigenIndex(Graph().decode(code).eigen(), eigens)

    @staticmethod
    def makeMultipliedCode(m, times=2):
        n, codes, _ = CodeGenerator.makeMinimumCode(m)

        if n * times > 100:
            times = int(100 / n)
        n *= times

        #allZero = '0' * int(n*(n-1)/2)
        #noise = Graph().decode(randomizeCode(0.2, allZero)).eigen()
        #for _ in range(99):
        #    noise += Graph().decode(randomizeCode(0.2, allZero)).eigen()
        #print(noise)
        #noise = 0.01 * noise
        #print(noise)

        graphs = [ Graph().decode(code).multiply(times) for code in codes ]
        codes = [ g.encode() for g in graphs ]
        eigens = [ g.eigen() for g in graphs ]
        #return n, codes, lambda code: Graph.findNearestEigenIndex(Graph().decode(code).eigen(), eigens)
        return n, codes, lambda code: Graph.findNearestEigenIndex(Graph().decode(smoothCode(code, 1)).eigen(), eigens)

    @staticmethod
    def makeForwardGrowCodeDecodedByEdgeNum(m, codeLen=1):
        n = 4
        for i in range(4, 101):
            if i*(i-1)/2/codeLen >= m:
                n = i
                break
        else:
            n = 100
            codeLen = int(n*(n-1)/2 / m)
            print('codeLen is truncated to {}'.format(codeLen), file=sys.stderr)

        codes = []
        for i in range(int(n*(n-1)/2/codeLen) + 1):
            code = '1' * i*codeLen + '0' * int((n*(n-1)/2) - i*codeLen)
            codes.append(code)

        codes = [codes[0]] + random.sample(codes[1:-2], m-2) + [codes[-1]]

        edgeNum = [ code.count('1') for code in codes ]
        return n, codes, lambda code: edgeNum.index(min(edgeNum, key=lambda num: abs(num - code.count('1'))))
        # reference for the lambda just above
        #   https://stackoverflow.com/questions/12141150/from-list-of-integers-get-number-closest-to-a-given-value

    @staticmethod
    def makeForwardGrowCodeDecodedByEigen(m, codeLen=3):
        n, codes, _ = CodeGenerator.makeForwardGrowCodeDecodedByEdgeNum(m, codeLen)
        graphs = [ Graph().decode(code) for code in codes ]
        eigens = [ g.eigen() for g in graphs ]
        return n, codes, lambda code: Graph.findNearestEigenIndex(Graph().decode(code).eigen(), eigens)

class Coder:
    def __init__(self, m, eps):
        self.m = m
        self.eps = eps

        if m == 10 and 0.04 <= eps and eps <= 0.1:
            if eps <= 0.04:
                self.n, self.codes, self.decoder = CodeGenerator.makeForwardGrowCodeDecodedByEdgeNum(m, 24)
            elif eps <= 0.09:
                self.n, self.codes, self.decoder = CodeGenerator.makeForwardGrowCodeDecodedByEigen(m, 68)
            else:
                self.n, self.codes, self.decoder = CodeGenerator.makeForwardGrowCodeDecodedByEigen(m, 34)
        else:
            self.n, self.codes, self.decoder = CodeGenerator.makeMinimumCode(m)
            #self.n, self.codes, self.decoder = CodeGenerator.makeMultipliedCode(m, 100)

    def generateCode(self):
        return '\n'.join([str(self.n)] + self.codes)

    def interpretCode(self, string):
        return self.decoder(string)

CodeToEigenN4 = {
    # N: 4, bit: 6, len: 11
    "000000" : (0.0, 0.0, 0.0, 0.0),
    "100000" : (-1.0, 0.0, 0.0, 1.0),
    "110000" : (-1.414214, 0.0, 0.0, 1.414214),
    "110100" : (-1.0, -1.0, 0.0, 2.0),
    "111000" : (-1.732051, 0.0, 0.0, 1.732051),
    "100001" : (-1.0, -1.0, 1.0, 1.0),
    "110010" : (-1.618034, -0.618034, 0.618034, 1.618034),
    "111100" : (-1.481194, -1.0, 0.311108, 2.170086),
    "110011" : (-2.0, -0.0, -0.0, 2.0),
    "111110" : (-1.561553, -1.0, -0.0, 2.561553),
    "111111" : (-1.0, -1.0, -1.0, 3.0),
}

CodeToEigenN5 = {
    # N: 5, bit: 10, len: 33
    "0000000000" : (0.0, 0.0, 0.0, 0.0, 0.0),
    "1000000000" : (-1.0, 0.0, 0.0, 0.0, 1.0),
    "1100000000" : (-1.414214, 0.0, 0.0, 0.0, 1.414214),
    "1100100000" : (-1.0, -1.0, 0.0, 0.0, 2.0),
    "1110000000" : (-1.732051, 0.0, 0.0, 0.0, 1.732051),
    "1000000100" : (-1.0, -1.0, 0.0, 1.0, 1.0),
    "1100010000" : (-1.618034, -0.618034, 0.0, 0.618034, 1.618034),
    "1110100000" : (-1.481194, -1.0, 0.0, 0.311108, 2.170086),
    "1111000000" : (-2.0, 0.0, 0.0, 0.0, 2.0),
    "1110110000" : (-1.561553, -1.0, -0.0, 0.0, 2.561553),
    "1110110100" : (-1.0, -1.0, -1.0, 0.0, 3.0),
    "1100000001" : (-1.414214, -1.0, 0.0, 1.0, 1.414214),
    "1110001000" : (-1.847759, -0.765367, 0.0, 0.765367, 1.847759),
    "1111100000" : (-1.813607, -1.0, 0.0, 0.470683, 2.342923),
    "1100010010" : (-1.732051, -1.0, -0.0, 1.0, 1.732051),
    "1110101000" : (-1.618034, -1.302776, -0.0, 0.618034, 2.302776),
    "1110001010" : (-2.135779, -0.662153, 0.0, 0.662153, 2.135779),
    "1111110000" : (-1.749118, -1.27133, 0.0, 0.334904, 2.685544),
    "1100100001" : (-1.0, -1.0, -1.0, 1.0, 2.0),
    "1110100001" : (-1.675131, -1.0, -0.539189, 1.0, 2.21432),
    "1110110010" : (-1.775713, -1.0, -0.589216, 0.723742, 2.641186),
    "1111110100" : (-1.514137, -1.0, -1.0, 0.428007, 3.08613),
    "1110001011" : (-2.44949, -0.0, -0.0, 0.0, 2.44949),
    "1111111000" : (-2.0, -1.0, -0.0, 0.0, 3.0),
    "1111100001" : (-1.561553, -1.0, -1.0, 1.0, 2.561553),
    "1100010011" : (-1.618034, -1.618034, 0.618034, 0.618034, 2.0),
    "1110101001" : (-2.0, -1.170086, -0.0, 0.688892, 2.481194),
    "1111110010" : (-1.618034, -1.472834, -0.462598, 0.618034, 2.935432),
    "1110110011" : (-2.17741, -1.0, 0.0, 0.321637, 2.855773),
    "1111111100" : (-1.681331, -1.0, -1.0, 0.357926, 3.323404),
    "1111110011" : (-2.0, -1.236068, -0.0, 0.0, 3.236068),
    "1111111110" : (-1.645751, -1.0, -1.0, 0.0, 3.645751),
    "1111111111" : (-1.0, -1.0, -1.0, -1.0, 4.0),
}

CodeToEigenN6 = {
    # N: 6, bit: 15, len: 151
    "000000000000000" : (0.0, 0.0, 0.0, 0.0, 0.0, 0.0),
    "100000000000000" : (-1.0, 0.0, 0.0, 0.0, 0.0, 1.0),
    "110000000000000" : (-1.414214, 0.0, 0.0, 0.0, 0.0, 1.414214),
    "110001000000000" : (-1.0, -1.0, 0.0, 0.0, 0.0, 2.0),
    "111000000000000" : (-1.732051, 0.0, 0.0, 0.0, 0.0, 1.732051),
    "100000000100000" : (-1.0, -1.0, 0.0, 0.0, 1.0, 1.0),
    "110000100000000" : (-1.618034, -0.618034, 0.0, 0.0, 0.618034, 1.618034),
    "111001000000000" : (-1.481194, -1.0, 0.0, 0.0, 0.311108, 2.170086),
    "111100000000000" : (-2.0, -0.0, -0.0, 0.0, 0.0, 2.0),
    "111001100000000" : (-1.561553, -1.0, 0.0, 0.0, 0.0, 2.561553),
    "111001100100000" : (-1.0, -1.0, -1.0, 0.0, 0.0, 3.0),
    "110000000000100" : (-1.414214, -1.0, 0.0, 0.0, 1.0, 1.414214),
    "111000010000000" : (-1.847759, -0.765367, 0.0, 0.0, 0.765367, 1.847759),
    "111101000000000" : (-1.813607, -1.0, 0.0, 0.0, 0.470683, 2.342923),
    "111000000000001" : (-1.732051, -1.0, 0.0, 0.0, 1.0, 1.732051),
    "111001010000000" : (-1.618034, -1.302776, 0.0, 0.0, 0.618034, 2.302776),
    "111000010010000" : (-2.135779, -0.662153, 0.0, 0.0, 0.662153, 2.135779),
    "111101100000000" : (-1.749118, -1.27133, 0.0, 0.0, 0.334904, 2.685544),
    "110001000000100" : (-1.0, -1.0, -1.0, 0.0, 1.0, 2.0),
    "111001000000100" : (-1.675131, -1.0, -0.539189, 0.0, 1.0, 2.21432),
    "111001100010000" : (-1.775713, -1.0, -0.589216, 0.0, 0.723742, 2.641186),
    "111101100100000" : (-1.514137, -1.0, -1.0, 0.0, 0.428007, 3.08613),
    "111000010010100" : (-2.44949, 0.0, 0.0, 0.0, 0.0, 2.44949),
    "111101110000000" : (-2.0, -1.0, -0.0, -0.0, 0.0, 3.0),
    "111101000000100" : (-1.561553, -1.0, -1.0, 0.0, 1.0, 2.561553),
    "110000100010100" : (-1.618034, -1.618034, 0.0, 0.618034, 0.618034, 2.0),
    "111001010000100" : (-2.0, -1.170086, -0.0, 0.0, 0.688892, 2.481194),
    "111101100010000" : (-1.618034, -1.472834, -0.462598, 0.0, 0.618034, 2.935432),
    "111001100010100" : (-2.17741, -1.0, -0.0, 0.0, 0.321637, 2.855773),
    "111101110100000" : (-1.681331, -1.0, -1.0, 0.0, 0.357926, 3.323404),
    "111101100010100" : (-2.0, -1.236068, -0.0, 0.0, 0.0, 3.236068),
    "111101110110000" : (-1.645751, -1.0, -1.0, 0.0, 0.0, 3.645751),
    "111101110110100" : (-1.0, -1.0, -1.0, -1.0, 0.0, 4.0),
    "111110000000000" : (-2.236068, -0.0, 0.0, 0.0, 0.0, 2.236068),
    "111100001000000" : (-2.074313, -0.835, -0.0, 0.0, 0.835, 2.074313),
    "111111000000000" : (-2.08613, -1.0, -0.0, 0.0, 0.571993, 2.514137),
    "110000000000110" : (-1.414214, -1.414214, 0.0, 0.0, 1.414214, 1.414214),
    "111000011000000" : ((-2+0j), (-1+0j), (-0-0j), (-0+0j), (1+0j), (2+0j)),
    "111000010000001" : (-1.902113, -1.175571, 0.0, 0.0, 1.175571, 1.902113),
    "111101001000000" : (-1.872276, -1.370326, -0.0, 0.0, 0.796815, 2.445787),
    "111100001001000" : (-2.288246, -0.874032, -0.0, 0.0, 0.874032, 2.288246),
    "111111100000000" : (-2.0, -1.342923, -0.0, 0.0, 0.529317, 2.813607),
    "110001000000110" : (-1.414214, -1.0, -1.0, 0.0, 1.414214, 2.0),
    "111001000000110" : (-1.89122, -1.0, -0.704624, -0.0, 1.317431, 2.278414),
    "111101001001000" : (-2.0, -1.0, -0.732051, -0.0, 1.0, 2.732051),
    "111111100100000" : (-1.855773, -1.0, -1.0, 0.0, 0.678363, 3.17741),
    "111000010010001" : (-2.236068, -1.0, -0.0, 0.0, 1.0, 2.236068),
    "111101101000000" : (-1.791288, -1.618034, -0.0, -0.0, 0.618034, 2.791288),
    "100000000100001" : (-1.0, -1.0, -1.0, 1.0, 1.0, 1.0),
    "110000100000001" : (-1.618034, -1.0, -0.618034, 0.618034, 1.0, 1.618034),
    "111000010001000" : (-1.931852, -1.0, -0.517638, 0.517638, 1.0, 1.931852),
    "111001010001000" : (-1.618034, -1.618034, -0.414214, 0.618034, 0.618034, 2.414214),
    "111001000000001" : (-1.481194, -1.0, -1.0, 0.311108, 1.0, 2.170086),
    "111101000000010" : (-1.920221, -1.0, -0.751024, 0.291367, 1.0, 2.379878),
    "110000100010010" : (-1.801938, -1.24698, -0.445042, 0.445042, 1.24698, 1.801938),
    "111000011010000" : (-2.24698, -0.801938, -0.554958, 0.554958, 0.801938, 2.24698),
    "111001010000010" : (-1.739677, -1.37379, -0.594523, 0.274205, 1.099585, 2.334201),
    "111101100001000" : (-1.894203, -1.329265, -0.60928, 0.306358, 0.772675, 2.753715),
    "111111000000100" : (-1.903212, -1.0, -1.0, 0.193937, 1.0, 2.709275),
    "111101101100000" : (-1.618034, -1.391382, -1.0, 0.227134, 0.618034, 3.164248),
    "111100001001010" : (-2.557612, -0.677214, 0.0, 0.0, 0.677214, 2.557612),
    "111111110000000" : (-2.123467, -1.322834, -0.0, 0.0, 0.344292, 3.102009),
    "111000010001001" : (-1.860806, -1.618034, -0.254102, 0.618034, 1.0, 2.114908),
    "111101001000010" : (-2.141987, -1.305298, -0.384753, 0.466856, 0.766109, 2.599072),
    "111111100010000" : (-1.824051, -1.618034, -0.54823, 0.328543, 0.618034, 3.043738),
    "111001000000101" : (-1.774623, -1.0, -1.0, 0.185885, 1.360409, 2.228328),
    "111101001000100" : (-1.67828, -1.319775, -1.0, 0.139666, 1.229738, 2.628651),
    "111001010000110" : (-2.136393, -1.206076, -0.540632, 0.261144, 1.082474, 2.539484),
    "111101101010000" : (-1.856343, -1.477984, -0.724824, 0.19669, 0.848136, 3.014326),
    "111101001001010" : (-2.311657, -1.0, -0.656954, 0.308777, 0.727166, 2.932668),
    "111111110100000" : (-1.86191, -1.282696, -1.0, 0.251197, 0.489727, 3.403682),
    "111000010010010" : (-2.175328, -1.126033, -0.0, 0.0, 1.126033, 2.175328),
    "111000011011000" : (-2.524338, -0.792287, 0.0, 0.0, 0.792287, 2.524338),
    "111101100000001" : (-1.851335, -1.350364, -0.55997, 0.0, 1.056082, 2.705587),
    "111101110001000" : (-2.112911, -1.0, -0.756244, 0.0, 0.821355, 3.047799),
    "111001010001100" : (-2.0, -1.561553, 0.0, 0.0, 1.0, 2.561553),
    "111101100001010" : (-2.240342, -1.368371, -0.0, 0.0, 0.664834, 2.943879),
    "111001100010001" : (-1.866198, -1.0, -1.0, 0.0, 1.210756, 2.655442),
    "111101100010010" : (-1.779608, -1.537083, -0.706201, 0.0, 1.041955, 2.980937),
    "111101110101000" : (-1.798457, -1.327912, -1.0, 0.0, 0.742438, 3.383931),
    "111111100010100" : (-2.0, -1.681331, 0.0, 0.0, 0.357926, 3.323404),
    "111001100010101" : (-2.292402, -1.0, -0.602705, 0.0, 1.0, 2.895107),
    "111101101010100" : (-2.139248, -1.29273, -0.597518, -0.0, 0.734704, 3.294791),
    "111111110110000" : (-1.766963, -1.384241, -1.0, 0.0, 0.440752, 3.710452),
    "111001100100001" : (-1.0, -1.0, -1.0, -1.0, 1.0, 3.0),
    "111101100100001" : (-1.704468, -1.0, -1.0, -0.508923, 1.116884, 3.096507),
    "111101110100001" : (-1.877404, -1.0, -1.0, -0.476452, 1.0, 3.353856),
    "111101110110010" : (-1.865307, -1.0, -1.0, -0.578441, 0.753441, 3.690306),
    "111111110110100" : (-1.53407, -1.0, -1.0, -1.0, 0.482696, 4.051374),
    "111100001001011" : (-2.828427, -0.0, -0.0, 0.0, 0.0, 2.828427),
    "111111111000000" : ((-2.372281+0j), (-1+0j), (-0-0j), (-0+0j), (-0+0j), (3.372281+0j)),
    "111101000000011" : (-2.191169, -1.0, -0.576713, -0.0, 1.264377, 2.503504),
    "111111100000001" : (-1.820822, -1.285865, -1.0, -0.0, 1.159274, 2.947413),
    "111000010010011" : (-2.164248, -1.618034, -0.0, 0.618034, 0.772866, 2.391382),
    "111101101000001" : (-2.0, -1.709275, -0.0, -0.0, 0.806063, 2.903212),
    "111101001000011" : (-2.454139, -1.195491, -0.0, 0.0, 0.853193, 2.796437),
    "111111110001000" : (-2.0, -1.540785, -0.512486, -0.0, 0.771876, 3.281394),
    "111101001001011" : (-2.626198, -1.0, -0.0, 0.0, 0.484862, 3.141336),
    "111111111100000" : (-2.141336, -1.0, -1.0, -0.0, 0.515138, 3.626198),
    "110000100010011" : (-2.0, -1.0, -1.0, 1.0, 1.0, 2.0),
    "111001010000011" : (-1.756598, -1.618034, -0.820249, 0.618034, 1.138564, 2.438283),
    "111101001000110" : (-2.0, -1.342923, -1.0, 0.529317, 1.0, 2.813607),
    "111101101011000" : (-1.618034, -1.618034, -1.236068, 0.618034, 0.618034, 3.236068),
    "111000011010010" : (-2.414214, -1.0, -0.414214, 0.414214, 1.0, 2.414214),
    "111101100001001" : (-2.158438, -1.271849, -0.661061, 0.183007, 1.055423, 2.852917),
    "111111100010010" : (-1.80853, -1.526602, -1.0, 0.112389, 1.0, 3.222743),
    "111001010001110" : (-2.220008, -1.618034, -0.231361, 0.618034, 0.710287, 2.741082),
    "111101101010001" : (-2.205168, -1.505788, -0.4663, 0.279848, 0.728212, 3.169195),
    "111111110101000" : (-1.751532, -1.618034, -1.0, 0.158917, 0.618034, 3.592615),
    "111000011011100" : (-2.732051, -0.732051, -0.0, 0.0, 0.732051, 2.732051),
    "111101110001010" : (-2.396206, -1.0, -0.627204, -0.0, 0.834653, 3.188757),
    "111101100001011" : ((-2.508622+0j), (-1.285519+0j), (-0-0j), (-0+0j), (0.701975+0j), (3.092166+0j)),
    "111111110001010" : (-2.176935, -1.47816, -0.528409, -0.0, 0.669443, 3.514061),
    "111101101010101" : (-2.471481, -1.338712, 0.0, 0.0, 0.349326, 3.460867),
    "111111111110000" : (-2.0, -1.292402, -1.0, 0.0, 0.397295, 3.895107),
    "110001000000111" : (-1.0, -1.0, -1.0, -1.0, 2.0, 2.0),
    "111001000000111" : (-1.732051, -1.0, -1.0, -0.414214, 1.732051, 2.414214),
    "111101001001100" : (-1.842236, -1.0, -1.0, -0.506942, 1.506942, 2.842236),
    "111001010000111" : (-2.0, -1.414214, -0.732051, -0.0, 1.414214, 2.732051),
    "111101101010010" : (-1.801938, -1.588364, -0.593579, -0.445042, 1.24698, 3.181943),
    "111101110101010" : (-2.0, -1.0, -1.0, -0.561553, 1.0, 3.561553),
    "111111100100001" : (-1.601679, -1.0, -1.0, -1.0, 1.339877, 3.261802),
    "111001100010011" : (-1.791288, -1.618034, -1.0, 0.618034, 1.0, 2.791288),
    "111101001001110" : (-2.178651, -1.173571, -1.0, 0.109626, 1.155797, 3.086799),
    "111101101100001" : (-2.0, -1.236068, -1.0, -0.0, 1.0, 3.236068),
    "111111110100001" : (-1.698969, -1.511053, -1.0, -0.407145, 1.082748, 3.534418),
    "111101100010011" : (-2.0, -1.618034, -0.860806, 0.618034, 0.745898, 3.114908),
    "111101101011100" : (-2.0, -1.581807, -0.79888, -0.0, 0.912755, 3.467932),
    "111101110101001" : (-2.190688, -1.187628, -1.0, 0.150544, 0.729919, 3.497854),
    "111111110110010" : (-1.78315, -1.475815, -1.0, -0.379146, 0.779159, 3.858951),
    "111101110110011" : (-2.279452, -1.0, -1.0, 0.0, 0.459363, 3.820089),
    "111111111110100" : ((-1.746568+0j), (-1-0j), (-1+0j), (-1+0j), (0.545096+0j), (4.201472+0j)),
    "111000011011110" : (-3.0, 0.0, 0.0, 0.0, 0.0, 3.0),
    "111101110001011" : (-2.717741, -1.0, -0.0, 0.0, 0.325397, 3.392344),
    "111111110001011" : ((-2.483612+0j), (-1.282824+0j), 0j, -0j, 0j, (3.766435+0j)),
    "111111111111000" : (-2.162278, -1.0, -1.0, -0.0, 0.0, 4.162278),
    "111101001001111" : (-2.372281, -1.0, -1.0, 0.0, 1.0, 3.372281),
    "111111111100001" : (-1.828427, -1.0, -1.0, -1.0, 1.0, 3.828427),
    "111111100010011" : (-1.618034, -1.618034, -1.44949, 0.618034, 0.618034, 3.44949),
    "111001010001111" : (-2.0, -2.0, 0.0, 0.0, 1.0, 3.0),
    "111101101010011" : (-2.24698, -1.575773, -0.554958, 0.187284, 0.801938, 3.38849),
    "111111110101001" : (-2.0, -1.489289, -1.0, 0.0, 0.710831, 3.778457),
    "111101101011110" : (-2.230725, -1.618034, -0.482859, 0.0, 0.618034, 3.713585),
    "111111111110010" : (-1.6874, -1.618034, -1.0, -0.431627, 0.618034, 4.119027),
    "111101110101011" : (-2.414214, -1.0, -1.0, 0.267949, 0.414214, 3.732051),
    "111111110110011" : (-2.184771, -1.244636, -1.0, 0.0, 0.361621, 4.067786),
    "111111111111100" : (-1.803542, -1.0, -1.0, -1.0, 0.375664, 4.427879),
    "111101101011111" : (-2.0, -2.0, 0.0, 0.0, 0.0, 4.0),
    "111111111110011" : (-2.0, -1.372281, -1.0, -0.0, 0.0, 4.372281),
    "111111111111110" : (-1.701562, -1.0, -1.0, -1.0, 0.0, 4.701562),
    "111111111111111" : (-1.0, -1.0, -1.0, -1.0, -1.0, 5.0),
}

class DictMaker:
    def __init__(self):
        pass

    @staticmethod
    def makeCodeToEigen(n):
        bit = int(n*(n-1)/2)
        g = Graph()
        codes = []
        eigens = []
        eigenToCode = {}
        eigenToDup = {}
        progress = 0
        print("makeCodeToEigen progress: ", end='', flush=True)
        for i in range(2**bit):
            formatStr = "{0:0" + str(bit) + "b}"
            code = formatStr.format(i)
            g.decode(code)
            eigen = g.eigen()
            eigenToCode[eigen] = code
            if eigenToDup.get(eigen) == None:
                eigenToDup[eigen] = 1
            else:
                eigenToDup[eigen] += 1
            if i / 2**bit * 100 > progress:
                print("{} ".format(progress), end='', flush=True)
                progress += 10
        print('100', flush=True)
        print('')

        codeToEigen = dict(zip(eigenToCode.values(), eigenToCode.keys()))
        duplication = list(eigenToDup.values())
        return codeToEigen, duplication

    @staticmethod
    def printCodeToEigen(n, codeToEigen):
        print('--------------------------------------------------------------------------')
        print('CodeToEigen')
        print('--------------------------------------------------------------------------')
        print("# n: {}, code length: {}, code num: {}".format(str(n), str(len(list(codeToEigen.keys())[0])), str(len(codeToEigen))))
        for code, eigen in codeToEigen.items():
            print("\"{}\" : {},".format(code, eigen))
        print()

    def printCodeInfo(codeToEigen, duplication):
        eigenToCode = dict(zip(codeToEigen.values(), codeToEigen.keys()))
        eigens = list(codeToEigen.values())

        print('--------------------------------------------------------------------------')
        print('CodeInfo')
        print('--------------------------------------------------------------------------')
        print('# index, code, numof \'1\', duplication, eigen')
        for i in range(len(eigens)):
            code = eigenToCode[eigens[i]]
            numofOne = code.count('1')
            print("{}, {}, {}, {}, {}, ".format(i, code, numofOne, duplication[i], eigens[i]))
        print()

    @staticmethod
    def printNorm(codeToEigen):
        eigens = list(codeToEigen.values())

        print('--------------------------------------------------------------------------')
        print('Norm')
        print('--------------------------------------------------------------------------')
        for i in range(len(eigens)):
            for j in range(len(eigens)):
                array1 = np.array(eigens[i])
                array2 = np.array(eigens[j])
                norm = LA.norm(array1 - array2)
                print("{}, ".format(str(norm)), end='')
            print()
        print()

def main():
    m, eps = input().split()
    m = int(m)
    eps = float(eps)

    coder = Coder(m, eps)
    print(coder.generateCode(), flush=True)

    for _ in range(100):
        Hk = input().split()[0]
        #print("Hk: {}".format(Hk), file=sys.stderr)
        print(coder.interpretCode(Hk), flush=True)

if __name__ == '__main__':
    if len(sys.argv) == 1:
        main()
    else:
        print('sys.argv: ' + str(sys.argv))

        #------------------------------------------------------------------------------------------
        #n = 4
        #codeToEigen, duplication = DictMaker.makeCodeToEigen(n)
        #DictMaker.printCodeToEigen(n, codeToEigen)
        #DictMaker.printCodeInfo(codeToEigen, duplication)
        #DictMaker.printNorm(codeToEigen)

        #------------------------------------------------------------------------------------------
        #m = 50
        #repeat = 3
        #scoreMatrix = []
        #errorMatrix = []
        #for codeLen in range(200, 201):
        #    #n, codes, decoder = CodeGenerator.makeForwardGrowCodeDecodedByEdgeNum(m, codeLen)
        #    #n, codes, decoder = CodeGenerator.makeForwardGrowCodeDecodedByEigen(m, codeLen)
        #    n, codes, decoder = CodeGenerator.makeMultipliedCode(m, 16)
        #    #n, codes, decoder = CodeGenerator.makeMinimumCode(m)
        #    print('n:', n)

        #    scores = []
        #    errors = []
        #    for eps in range(20, 21):
        #        eps = eps * 0.01
        #        score, error = simulateScore(eps, n, codes, decoder, repeat)
        #        scores.append(score)
        #        errors.append(error)
        #    scoreMatrix.append(scores)
        #    errorMatrix.append(errors)

        #print('m: {}'.format(m))
        #print('Score:')
        #for i in range(len(scoreMatrix)):
        #    for j in range(len(scoreMatrix[0])):
        #        print("{}, ".format(scoreMatrix[i][j]), end='')
        #    print()
        #print()

        #print('Error:')
        #for i in range(len(errorMatrix)):
        #    for j in range(len(errorMatrix[0])):
        #        print("{}, ".format(errorMatrix[i][j]), end='')
        #    print()
        #print()

        #------------------------------------------------------------------------------------------
        #eps = 0.2
        #n = 16
        #codeLen = int(n*(n-1)/2)
        ##code = '1' * codeLen
        #code = '000111100000000001111000000000111100000000111100000000000000000000000000000000000000000000000001111001111011111111000000'
        ##code_r = randomizeCode(eps, code)
        #code_r = '100110001000000110110001000101110000110101000101010000000101001100010110010101010010000000101111001100011001000001011001'
        #code_s = smoothCode(code_r, 1)

        #print(Graph().decode(code))
        #print()
        #print(Graph().decode(code_r))
        #print()
        #print(Graph().decode(code_s))
        #print()
        
