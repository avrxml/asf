function [e, y, w]  = nlms(u, d, a, b, w0)

% [e, y, w] = nlms(u, d, a, b, w0) は，
% NLMS (normalized least mean square)法を用いて1次元FIR適応フィルタを実現する．
%
%   u  : 入力信号 (1xK)
%   d  : 参照(目的)信号 (1xK)
%   a  : パラメータ (正の実数)
%   b  : パラメータ (正の実数)
%   w0 : フィルタ係数ベクトルの初期値 (Lx1)
%   e  : 誤差信号 (1xK)
%   y  : 出力信号 (1xK)
%   w  : フィルタ係数ベクトルの時系列 (LxK)
%
%   NLMS アルゴリズム : wnew = w + 2 * a * x * e / (x'*x + b)
%
% 2000.12.01  OHKI Makoto

K = length(u);
L = length(w0);
u = [zeros(1, L-1), u];
e = zeros(1, K);
y = zeros(1, K);
w = zeros(L, K+1);
w(:,1) = w0;
for k=1:K
  x = u(k+L-1:-1:k)';
  y(k) = x' * w(:, k);
  e(k) = d(k) - y(k);
  w(:, k+1) = w(:, k) + 2 * a * x * e(k) / (x' * x + b);
end
w = w(:, 1:K);
