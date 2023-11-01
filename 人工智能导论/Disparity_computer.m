I_r = imread('./cone/im0.png'); % right-view (image where objects are at right)
I_l = imread('./cone/im1.png'); % left-view (image where objects are at left)

JR = rgb2gray(I_r);
JL = rgb2gray(I_l);
[size_x, size_y] = size(JL);
% Hyper parameters
pooling = 2;
% matching window size
w = 19;
% max offset of pixel permitted
ndisp = size_x / 6;


Disp = zeros(size_x, size_y);
fbar = waitbar(0,'computing disparity map');
for i=1:pooling:size_x-w
    waitbar(i/(size_x-w), fbar, 'computing disparity map');
    for j=1:pooling:size_y-w
        [i_,j_] = find_pos(i,j,JL,JR,w);
        disp_value = abs(j_ - j);
        % release noise from computing
        if disp_value > ndisp
            disp_value = 0;
        end
        % pooling
        Disp(i:i+(pooling-1),j:j+(pooling-1)) = disp_value;
    end
end
close(fbar);
delete(fbar);

figure
imshow(mat2gray(Disp));
colormap jet
title('Self designed algorithm')
colorbar

% Using integrate function
% Need Computer Vision Toolbox 
disparityRange = [0 128];
disparityMap = disparityBM(JR,JL,'DisparityRange',disparityRange,'UniquenessThreshold',10);
figure
imshow(disparityMap,disparityRange)
title('CV tool function')
colormap jet
colorbar

% Single point detection
% Just for debug
% x = 200;
% y = 900;
% [x_,y_] = find_pos(x, y, JL, JR, w);
% JL(x:x+w,y:y+w)=200;
% figure
% imshow(JL)
% figure
% JR(x_:x_+w,y_:y_+w)=200;
% imshow(JR)

function r=correlation(A, B)
    r = sum(sum(abs(int16(A)-int16(B))));
end

function [x_,y_]=find_pos(x,y,JL,JR,w)
% Input image where scene shifts left
    [~, size_y] = size(JR);
    A = JL(x:x+w,y:y+w);
    min_corr = inf;
    x_ = x;
    y_ = 1;
    for j=y:(size_y-w)
        % B lies at A's RHS
        B = JR(x:x+w,j:j+w);
        corr = correlation(A,B);
        if corr <= min_corr
            min_corr = corr;
            y_ = j;
        end
    end
end
    