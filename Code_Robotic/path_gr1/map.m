I = imread('map.png');
I = rgb2gray(I);
J = imresize(I,[202 302]);
BW = imbinarize(J);
BW = BW-1;
BW = abs(BW*(-1));
fid = fopen('Mymatrix.txt','wt');
for ii = 1:size(BW,1)
    fprintf(fid,'%g',BW(ii,:));
end
fclose(fid);
imshow(BW);